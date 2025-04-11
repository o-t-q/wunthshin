// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Skill/O_WSBaseSkill.h"
#include "O_WSSlashingSkill.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UO_WSSlashingSkill : public UO_WSBaseSkill
{
	GENERATED_BODY()

public:
	virtual void DoSkillImpl(const FSkillParameter& InParameter, ICommonPawn* InInstigator, const FVector& InTargetLocation, AActor* InTargetActor) override;
};
