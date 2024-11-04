#include "WSDamageEvent.h"
#include "wunthshin/Subsystem/WorldStatusSubsystem/WorldStatusSubsystem.h"

bool FWSDamageEvent::IsFirstHit(const ICommonPawn* InPawn) const
{
	check(IsValid());
	const UObject* Object = Cast<UObject>(InPawn);
		
	if (const UWorld* World = Object->GetWorld())
	{
		return !World->GetSubsystem<UWorldStatusSubsystem>()->IsDamageTakenBy(InPawn, AttackID);
	}
		
	check(Object);
	return false;
}

void FWSDamageEvent::SetFirstHit(const ICommonPawn* InPawn) const
{
	check(IsValid());
	const UObject* Object = Cast<UObject>(InPawn);

	if (const UWorld* World = Object->GetWorld())
	{
		if (UWorldStatusSubsystem* WorldStatusSubsystem = World->GetSubsystem<UWorldStatusSubsystem>())
		{
			WorldStatusSubsystem->SetDamageTaken(InPawn, AttackID);
		}
	}
}
