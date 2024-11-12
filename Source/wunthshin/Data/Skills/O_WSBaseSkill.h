// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTableRow/SkillTableRow.h"
#include "UObject/Object.h"

#include "wunthshin/Data/Elements/ElementRowHandle/ElementRowHandle.h"

#include "O_WSBaseSkill.generated.h"

class ICommonPawn;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UO_WSBaseSkill : public UObject
{
	GENERATED_BODY()

public:
	virtual void DoSkillImpl(const FSkillParameter& InParameter, ICommonPawn* InInstigator, const FVector& InTargetLocation, AActor* InTargetActor) PURE_VIRTUAL(UO_WSBaseSkill::DoSkillImpl, );
	void DoSkill(const FSkillParameter& InParameter, ICommonPawn* InInstigator, const FVector& InTargetLocation, AActor* InTargetActor);
};
