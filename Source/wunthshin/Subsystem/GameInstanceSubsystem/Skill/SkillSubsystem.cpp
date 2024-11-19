// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSubsystem.h"

#include "LevelSequencePlayer.h"
#include "wunthshin/Data/Skills/O_WSBaseSkill.h"
#include "wunthshin/Data/Skills/SkillRowHandle/SkillRowHandle.h"
#include "wunthshin/Data/Skills/SkillTableRow/SkillTableRow.h"
#include "wunthshin/Interfaces/ElementTracked/ElementTracked.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/EventTicket/SkillTicket/SkillTickets.h"

DEFINE_LOG_CATEGORY(LogSkillSubsystem);

USkillSubsystem::USkillSubsystem()
	: DataTable(nullptr) {}

void USkillSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_SkillTable.DT_SkillTable'")));
	check(DataTable);
	
	DataTableMapping.Emplace(FSkillTableRow::StaticStruct(), DataTable);

	TArray<FSkillTableRow*> Rows;
	DataTable->GetAllRows<FSkillTableRow>(TEXT(""), Rows);

	for (const FSkillTableRow* Row : Rows)
	{
		if (!PreinstantiatedSkillProcessor.Contains(Row->SkillProcessor))
		{
			UE_LOG(LogSkillSubsystem, Log, TEXT("%s: Skill Processor added"), *Row->SkillProcessor->GetName());
			PreinstantiatedSkillProcessor.Add(Row->SkillProcessor);
			PreinstantiatedSkillProcessor[Row->SkillProcessor] = NewObject<UO_WSBaseSkill>(this, Row->SkillProcessor);
		}
	}
}

bool USkillSubsystem::CastSkill(
	const FSkillRowHandle& InSkill, ICommonPawn* InInstigator, const FVector& InTargetLocation, AActor* InTargetActor
)
{
	if (UWorldStatusSubsystem* WorldStatusSubsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
	{
		if (WorldStatusSubsystem->IsLevelSequencePlaying())
		{
			return false;
		}

		TSharedPtr<FSkillStartTicket> SkillTicket = MakeShared<FSkillStartTicket>();
		SkillTicket->SkillHandle = InSkill;
		SkillTicket->Instigator = InInstigator;
		SkillTicket->TargetLocation = InTargetLocation;
		SkillTicket->TargetActor = InTargetActor;
		
		WorldStatusSubsystem->PushTicket(SkillTicket);

		return true;
	}

	return false;
}

UO_WSBaseSkill* USkillSubsystem::GetSkillProcessor(TSubclassOf<UO_WSBaseSkill> SkillClass)
{
	if (!PreinstantiatedSkillProcessor.Contains(SkillClass))
	{
		return nullptr;
	}
	
	return PreinstantiatedSkillProcessor[SkillClass];
}

UO_WSBaseSkill* USkillSubsystem::GetSkillProcessor(const FSkillRowHandle& InSkill)
{
	const FSkillTableRow* SkillValue = InSkill.Handle.GetRow<FSkillTableRow>(TEXT(""));

	if (SkillValue != nullptr)
	{
		GetSkillProcessor(SkillValue->SkillProcessor);
	}

	return nullptr;
	
}
