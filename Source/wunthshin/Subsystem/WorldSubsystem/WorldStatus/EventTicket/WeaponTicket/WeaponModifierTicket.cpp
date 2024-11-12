#include "WeaponModifierTicket.h"

#include "wunthshin/Components/Weapon/C_WSWeapon.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"

DEFINE_LOG_CATEGORY(LogWeaponModifier);

void FWeaponModifierTicket::Execute(UWorld* InWorld)
{
	if (WeaponComponent)
	{
		UE_LOG(LogWeaponModifier, Log, TEXT("Weapon %s modifier: Damage %f, Speed %f"), *WeaponComponent->GetOwner()->GetName(), DamageModifier, AttackSpeed);
		WeaponComponent->SetDamageModifier(DamageModifier);
		WeaponComponent->SetAttackSpeed(AttackSpeed);

		if (UWorldStatusSubsystem* WorldStatusSubsystem = InWorld->GetSubsystem<UWorldStatusSubsystem>())
		{
			TSharedPtr<FWeaponModifierFinishTicket> EndTicket = MakeShared<FWeaponModifierFinishTicket>();
			EndTicket->WeaponComponent = WeaponComponent;
			WorldStatusSubsystem->PushTicketScheduled(EndTicket, BuffTimerHandle, BuffDuration);
		}
	}

	SetDisposed();
}

void FWeaponModifierFinishTicket::Execute(UWorld* InWorld)
{
	if (WeaponComponent)
	{
		UE_LOG(LogWeaponModifier, Log, TEXT("Reset weapon modifier for %s"), *WeaponComponent->GetOwner()->GetName())
		WeaponComponent->ResetAttackSpeed();
		WeaponComponent->ResetDamageModifier();
	}

	SetDisposed();
}
