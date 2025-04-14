#include "Subsystem/EventTicket/WeaponTicket/WeaponModifierTicket.h"

#include "Component/C_WSWeapon.h"
#include "Subsystem/WorldStatusSubsystem.h"

DEFINE_LOG_CATEGORY(LogWeaponModifier);

void FWeaponModifierTicket::Execute(UWorld* InWorld)
{
	if (WeaponComponent)
	{
		UE_LOG(LogWeaponModifier, Log, TEXT("Weapon %s modifier"), *WeaponComponent->GetOwner()->GetName());
		WeaponComponent->GetWeaponContext().SetModifier(WeaponModifier);
		
		if (UWorldStatusSubsystem* WorldStatusSubsystem = InWorld->GetSubsystem<UWorldStatusSubsystem>())
		{
			TSharedPtr<FWeaponModifierRevokeTicket> EndTicket = MakeShared<FWeaponModifierRevokeTicket>();
			EndTicket->WeaponComponent = WeaponComponent;
			WorldStatusSubsystem->PushTicketScheduled(EndTicket, BuffTimerHandle, BuffDuration);
			return;
		}
	}

	SetDisposed();
}

void FWeaponModifierRevokeTicket::Execute(UWorld* InWorld)
{
	if (WeaponComponent)
	{
		UE_LOG(LogWeaponModifier, Log, TEXT("Reset weapon modifier for %s"), *WeaponComponent->GetOwner()->GetName())
		WeaponComponent->GetWeaponContext().ResetModifier();
		return;
	}

	SetDisposed();
}
