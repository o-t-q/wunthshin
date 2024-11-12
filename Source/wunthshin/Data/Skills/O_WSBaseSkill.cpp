// Fill out your copyright notice in the Description page of Project Settings.


#include "O_WSBaseSkill.h"

#include "wunthshin/Components/Skill/C_WSSkill.h"
#include "wunthshin/Interfaces/CommonPawn/CommonPawn.h"

void UO_WSBaseSkill::DoSkill(
	const FSkillParameter& InParameter, ICommonPawn* InInstigator, const FVector& InTargetLocation,
	AActor* InTargetActor
)
{
	DoSkillImpl(InParameter, InInstigator, InTargetLocation, InTargetActor);
	InInstigator->GetSkillComponent()->SetSkillActive(false);
}
