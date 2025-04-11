#pragma once
#include "Data/Skill/SkillRowHandle.h"
#include "Subsystem/EventTicket/EventTicket.h"

class UO_WSBaseSkill;
class ICommonPawn;

struct WUNTHSHIN_API FSkillStartTicket : public FEventTicket
{
public:
	FSkillRowHandle SkillHandle;
	ICommonPawn* Instigator;
	FVector TargetLocation;
	AActor* TargetActor;
	
	virtual void Execute(UWorld* InWorld) override;
};

struct WUNTHSHIN_API FSkillRevokeTicket : public FEventTicket
{
public:
	FSkillRowHandle SkillHandle;
    ICommonPawn* Instigator;
    FVector TargetLocation;
    AActor* TargetActor;
	UO_WSBaseSkill* SkillProcessor;
	
	virtual void Execute(UWorld* InWorld) override;
};