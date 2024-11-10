// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "wunthshin/Data/Skills/SkillRowHandle/SkillRowHandle.h"
#include "wunthshin/Data/Skills/SkillTableRow/SkillTableRow.h"
#include "wunthshin/Interfaces/CommonPawn/CommonPawn.h"
#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "SkillSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSkillSubsystem, Log, All);

class UO_WSBaseSkill;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API USkillSubsystem : public UGameInstanceSubsystem, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<UO_WSBaseSkill>, UO_WSBaseSkill*> PreinstantiatedSkillProcessor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta = (AllowPrivateAccess = "true"))
	UDataTable* DataTable;

public:
	USkillSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	bool CastSkill(
		const FSkillRowHandle& InSkill, ICommonPawn* InInstigator, const FVector& InTargetLocation,
		AActor*                InTargetActor
	);
	
	UO_WSBaseSkill* GetSkillProcessor(TSubclassOf<UO_WSBaseSkill> SkillClass);
	UO_WSBaseSkill* GetSkillProcessor(const FSkillRowHandle& InSkill);
};
