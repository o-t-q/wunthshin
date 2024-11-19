﻿#pragma once
#include "wunthshin/Data/Skills/SkillRowHandle/SkillRowHandle.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/EventTicket/EventTicket.h"

class UO_WSBaseSkill;
class ICommonPawn;

struct FSkillStartTicket : public FEventTicket
{
public:
	FSkillRowHandle SkillHandle;
	ICommonPawn* Instigator;
	FVector TargetLocation;
	AActor* TargetActor;
	
	virtual void Execute(UWorld* InWorld) override;
};

struct FSkillRevokeTicket : public FEventTicket
{
public:
	FSkillRowHandle SkillHandle;
    ICommonPawn* Instigator;
    FVector TargetLocation;
    AActor* TargetActor;
	UO_WSBaseSkill* SkillProcessor;
	
	virtual void Execute(UWorld* InWorld) override;
};