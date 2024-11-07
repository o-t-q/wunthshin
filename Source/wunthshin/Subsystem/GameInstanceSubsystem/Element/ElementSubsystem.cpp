// Fill out your copyright notice in the Description page of Project Settings.


#include "ElementSubsystem.h"
#include "wunthshin/Data/Elements/ElementTableRow/ElementTableRow.h"
#include "wunthshin/Data/Elements/O_WSElementReactor.h"
#include "wunthshin/Interfaces/ElementTracked/ElementTracked.h"

DEFINE_LOG_CATEGORY(LogElementSubsystem);

uint32 GetTypeHash(const FElementRowHandle& InDataTableHandle)
{
	return CityHash32(reinterpret_cast<const char*>(&InDataTableHandle), sizeof(InDataTableHandle));
}

UElementSubsystem::UElementSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ElementTable.DT_ElementTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

FElementRowHandle UElementSubsystem::GetElementHandle(const UWorld* InWorld, const FName& ElementName)
{
	const FDataTableRowHandle DataTableRowHandle = InWorld->GetGameInstance()->GetSubsystem<UElementSubsystem>()->GetRowHandle(FElementTableRow::StaticStruct(), ElementName);
	ensure(!DataTableRowHandle.IsNull());
	
	return FElementRowHandle(DataTableRowHandle);
}

void UElementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ensure(DataTable);
	DataTableMapping.Emplace(FElementTableRow::StaticStruct(), DataTable);

	// reactor를 매번 사용할 때마다 instantiate하지 않고 한번 생성해서
	// 불러와 사용하도록, 서브시스템에서 미리 생성
	TArray<FElementTableRow*> Rows;
	DataTable->GetAllRows<FElementTableRow>(TEXT(""), Rows);

	// 전수 탐색 후 reactor 생성
	for (const FElementTableRow* Row : Rows) 
	{
		for (const FElementReaction& Reaction : Row->Reactions) 
		{
			if (!PreinstantiatedReactors.Contains(Reaction.Reactor)) 
			{
				UE_LOG(LogElementSubsystem, Log, TEXT("%s: Reactor added"), *Reaction.Reactor->GetName());
				PreinstantiatedReactors.Add(Reaction.Reactor);
				PreinstantiatedReactors[Reaction.Reactor] = NewObject<UO_WSElementReactor>(this, Reaction.Reactor);
			}
		}
	}
}

void UElementSubsystem::ApplyElement(AActor* InTarget, AActor* InInstigator, const FElementRowHandle& InElementRow)
{
	// 빈 요청
	if (InElementRow.Handle.IsNull())
	{
		return;
	}
	
	UE_LOG(LogElementSubsystem, Log, TEXT("Applying the element %s to %s by %s"), *InElementRow.Handle.RowName.ToString(), *InTarget->GetName(), *InInstigator->GetName());

	if (TrackingObjects.Contains(InTarget)) 
	{
		FElementTrackingMap& Map = TrackingObjects[InTarget];
		
		// 중복된 원소효과는 해당 원소의 타이머를 갱신
		if (Map.Contains(InElementRow))
		{
			UE_LOG(LogElementSubsystem, Log, TEXT("Element %s exists, extending the timer"), *InElementRow.Handle.RowName.ToString());
			Map.ResetTimer(InInstigator->GetWorld(), InElementRow);
		}
		
		Map.Add(InInstigator->GetWorld(), InInstigator, InElementRow);

		if (IElementTracked* Interface = Cast<IElementTracked>(InTarget))
		{
			Interface->OnElementApplied.Broadcast(InElementRow);	
		}

		if (Map.IsFull()) 
		{
			const FElementReactionPair& Pair = Map.ExtractElement();
			UO_WSElementReactor* Reactor = GetReactor(Pair.Elements[0], Pair.Elements[1]);

			if (!Reactor)
			{
				// 매칭되는 요소의 반응이 없는 경우
				ensure(false);
				return;
			}
			
			if (Pair.Instigators.Num() != 2) 
			{
				// element를 적용한 대상이 두명이 아닌 경우 
				// (둘 다 같은 instigator는 예외, 같은 Instigator가 다른 속성 두개를 부여할 수 있음)
				ensure(false);
				return;
			}
			
			AActor* OtherInstigator = Pair.Instigators[0] == InInstigator ? Pair.Instigators[1] : Pair.Instigators[0];
			UE_LOG(LogElementSubsystem, Log, TEXT("%s effect triggered by %s and %s!"), *Reactor->GetName(), *InInstigator->GetName(), *OtherInstigator->GetName());
			Reactor->React(InInstigator, OtherInstigator, InTarget);

			// 지금까지 추적하던 원소 상태 초기화
			TrackingObjects.Remove(InTarget);
		}
	}
	else
	{
		// 처음 부여된 상태이면 효과가 발생할 수 없음 (2개의 다른 조합으로 효과 발생)
		FElementTrackingMap& NewMap = TrackingObjects.Add(InTarget);
		NewMap.Add(InInstigator->GetWorld(), InInstigator, InElementRow);
	}
}

UO_WSElementReactor* UElementSubsystem::GetReactor(const FElementRowHandle& InLeft, const FElementRowHandle& InRight) const
{
	const FElementTableRow* LeftElement = InLeft.Handle.GetRow<FElementTableRow>(TEXT(""));
	const FElementReaction* LRReaction = LeftElement->Reactions.FindByPredicate([&InRight](const FElementReaction& InReaction) {
		return InReaction.OtherElement == InRight.Handle;
		});

	// 찾고자하는 좌우 반응 효과가 없는 경우
	ensure(LRReaction);

	if (LRReaction) 
	{
		return GetReactor(LRReaction->Reactor);
	}

	return nullptr;
}

UO_WSElementReactor* UElementSubsystem::GetReactor(TSubclassOf<UO_WSElementReactor> InReactorType) const
{
	const bool bExist = PreinstantiatedReactors.Contains(InReactorType);
	// 알려지지 않은 reactor를 요청한 경우
	ensure(bExist);

	if (bExist) 
	{
		return PreinstantiatedReactors[InReactorType];
	}

	return nullptr;
}
