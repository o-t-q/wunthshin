// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSubsystem.h"

#include "LevelSequencePlayer.h"
#include "wunthshin/Data/Skills/O_WSBaseSkill.h"
#include "wunthshin/Data/Skills/SkillRowHandle/SkillRowHandle.h"
#include "wunthshin/Data/Skills/SkillTableRow/SkillTableRow.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"

DEFINE_LOG_CATEGORY(LogSkillSubsystem);

USkillSubsystem::USkillSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_SkillTable.DT_SkillTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

void USkillSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
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

void USkillSubsystem::CastSkill(const FSkillRowHandle& InSkill, ICommonPawn* InInstigator, const FVector& InTargetLocation, AActor* InTargetActor)
{
	if (const FSkillTableRow* Skill = InSkill.Handle.GetRow<FSkillTableRow>(TEXT("")))
	{
		if (Skill->Parameter.CastingSequence)
		{
			if (UWorldStatusSubsystem* WorldStatusSubsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
			{
				if (WorldStatusSubsystem->IsLevelSequencePlaying())
				{
					return;
				}
				
				UO_WSBaseSkill* SkillProcessor = PreinstantiatedSkillProcessor[Skill->SkillProcessor];
				// 미리 스킬 처리기가 생성되지 않은 경우
				check(SkillProcessor);

				const TFunction<void()> SkillProcessorFunction = [Skill, InInstigator, SkillProcessor, InTargetLocation, InTargetActor]()
				{
					SkillProcessor->DoSkill(Skill->Parameter, InInstigator, InTargetLocation, InTargetActor);	
				};
				
				WorldStatusSubsystem->PlayLevelSequence(Skill->Parameter.CastingSequence, SkillProcessorFunction);
			}
		}
	}
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
