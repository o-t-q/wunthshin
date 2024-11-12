#pragma once
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/EventTicket/EventTicket.h"

class USG_WSItemMetadata;

struct FItemTicket : public FEventTicket
{
	friend class UWorldStatusSubsystem;

	const USG_WSItemMetadata* Item;
	AActor* Instigator;
	AActor* Target;

private:
	virtual void Execute(UWorld* InWorld) override;

	uint32 ExecuteCount = 0;
	uint32 MaxExecuteCount = 0;
};
