#pragma once
#include "wunthshin/Data/Elements/ElementRowHandle/ElementRowHandle.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/EventTicket/EventTicket.h"

DECLARE_LOG_CATEGORY_EXTERN(LogElementTicket, Log, All);

struct FElementReactTicket : public FEventTicket
{
public:
	FElementRowHandle ElementHandle;
	AActor* Instigator;
	AActor* TargetActor;
	
	virtual void Execute(UWorld* InWorld) override;
};

struct FElementReactFinishTicket : public FEventTicket
{
public:
	AActor* TargetActor;
	
	virtual void Execute(UWorld* InWorld) override;
};