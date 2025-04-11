#pragma once
#include "Data/Element/ElementRowHandle.h"
#include "Subsystem/EventTicket/EventTicket.h"

DECLARE_LOG_CATEGORY_EXTERN(LogElementTicket, Log, All);

struct WUNTHSHIN_API FElementReactTicket : public FEventTicket
{
public:
	FElementRowHandle ElementHandle;
	AActor* Instigator;
	AActor* TargetActor;
	
	virtual void Execute(UWorld* InWorld) override;
};

struct WUNTHSHIN_API FElementReactRevokeTicket : public FEventTicket
{
public:
	AActor* TargetActor;
	
	virtual void Execute(UWorld* InWorld) override;
};