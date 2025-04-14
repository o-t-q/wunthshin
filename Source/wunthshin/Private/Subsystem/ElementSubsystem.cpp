// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ElementSubsystem.h"
#include "Data/Element/ElementTableRow.h"
#include "Data/Element/O_WSElementReactor.h"
#include "Interface/ElementTracked.h"
#include "Subsystem/WorldStatusSubsystem.h"
#include "Subsystem/EventTicket/ElementTicket/ElementReactTicket.h"

DEFINE_LOG_CATEGORY(LogElementSubsystem);

UElementSubsystem::UElementSubsystem()
	: DataTable(nullptr) {}

FElementRowHandle UElementSubsystem::GetElementHandle(const UWorld* InWorld, const FName& ElementName)
{
	const FDataTableRowHandle DataTableRowHandle = InWorld->GetGameInstance()->GetSubsystem<UElementSubsystem>()->GetRowHandle(FElementTableRow::StaticStruct(), ElementName);
	ensure(!DataTableRowHandle.IsNull());
	
	return FElementRowHandle(DataTableRowHandle);
}

void UElementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), this, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ElementTable.DT_ElementTable'")));
	check(DataTable);
	
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
	if (UWorldStatusSubsystem* WorldStatusSubsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
	{
		TSharedPtr<FElementReactTicket> ReactTicket = MakeShared<FElementReactTicket>();
		ReactTicket->Instigator = InInstigator;
		ReactTicket->ElementHandle = InElementRow;
		ReactTicket->TargetActor = InTarget;

		WorldStatusSubsystem->PushTicket(ReactTicket);
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
