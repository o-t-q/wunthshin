// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Skill/O_WSBaseSkill.h"

#include "Component/C_WSSkill.h"
#include "Interface/CommonPawn.h"

void UO_WSBaseSkill::DoSkill(
	const FSkillParameter& InParameter, ICommonPawn* InInstigator, const FVector& InTargetLocation,
	AActor* InTargetActor
)
{
	DoSkillImpl(InParameter, InInstigator, InTargetLocation, InTargetActor);
	InInstigator->GetSkillComponent()->SetSkillActive(false);
}
