// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshin/Subsystem/ElementSubsystem/ElementSubsystem.h"
#include "wunthshin/Data/ElementTableRow/ElementTableRow.h"
#include "wunthshin/Data/Elements/O_WSElementReactor.h"

DEFINE_LOG_CATEGORY(LogElementSubsystem);

uint32 GetTypeHash(const FDataTableRowHandle& InDataTableHandle)
{
	return CityHash32(reinterpret_cast<const char*>(&InDataTableHandle), sizeof(InDataTableHandle));
}

UElementSubsystem::UElementSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ElementTable.DT_ElementTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

void UElementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ensure(DataTable);

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

void UElementSubsystem::ApplyElement(AActor* InTarget, AActor* InInstigator, const FDataTableRowHandle& InElementRow)
{
	if (TrackingObjects.Contains(InTarget)) 
	{
		FElementTrackingMap& Map = TrackingObjects[InTarget];
		Map.Add(InTarget->GetWorld(), InInstigator, InElementRow);

		if (Map.IsFull()) 
		{
			const FElementReactionPair& Pair = Map.ExtractElement();
			UO_WSElementReactor* Reactor = GetReactor(Pair.Elements[0], Pair.Elements[1]);

			if (Pair.Instigators.Num() != 2) 
			{
				// element를 적용한 대상이 두명이 아닌 경우 
				// (둘 다 같은 instigator는 예외, 같은 Instigator가 다른 속성 두개를 부여할 수 있음)
				ensure(false);
				return;
			}
			
			AActor* OtherInstigator = Pair.Instigators[0] == InInstigator ? Pair.Instigators[1] : Pair.Instigators[0];
			Reactor->React(InInstigator, OtherInstigator, InTarget);

			// 지금까지 추적하던 원소 상태 초기화
			TrackingObjects.Remove(InTarget);
		}
	}
	else
	{
		// 처음 부여된 상태이면 효과가 발생할 수 없음 (2개의 다른 조합으로 효과 발생)
		FElementTrackingMap& NewMap = TrackingObjects.Add(InTarget);
		NewMap.Add(InTarget->GetWorld(), InInstigator, InElementRow);
	}
}

UO_WSElementReactor* UElementSubsystem::GetReactor(const FDataTableRowHandle& InLeft, const FDataTableRowHandle& InRight)
{
	const FElementTableRow* LeftElement = InLeft.GetRow<FElementTableRow>(TEXT(""));
	const FElementReaction* LRReaction = LeftElement->Reactions.FindByPredicate([&InRight](const FElementReaction& InReaction) {
		return InReaction.OtherElement == InRight;
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
