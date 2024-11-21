// Fill out your copyright notice in the Description page of Project Settings.


#include "O_WSSlashingSkill.h"

#include "wunthshin/Data/Items/DamageEvent/WSDamageEvent.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/EventTicket/WeaponTicket/WeaponModifierTicket.h"

void UO_WSSlashingSkill::DoSkillImpl(const FSkillParameter& InParameter, ICommonPawn* InInstigator,
                                     const FVector& InTargetLocation, AActor* InTargetActor)
{
	if (InParameter.FloatValues.Contains("Damage"))
	{
		const float   Damage = InParameter.FloatValues["Damage"];
		const UWorld* World  = InTargetActor->GetWorld();
		AController* Instigator = Cast<AActor>(InInstigator)->GetInstigatorController();

		if (UWorldStatusSubsystem* WorldStatusSubsystem = World->GetSubsystem<UWorldStatusSubsystem>())
		{
			WorldStatusSubsystem->PushAttack(this);

			FWSDamageEvent DamageEvent{};
			DamageEvent.SetDamageEvent(WorldStatusSubsystem->GetCurrentAttackID(this));
			InTargetActor->TakeDamage(Damage, DamageEvent, Instigator, nullptr);

			// 즉발이므로 바로 추적 해제
			WorldStatusSubsystem->PopAttack(this);
			
			if (AActor* Weapon = InInstigator->GetRightHandComponent()->GetChildActor())
			{
				const TSharedPtr<FWeaponModifierTicket> ModifierTicket = MakeShared<FWeaponModifierTicket>();
				ModifierTicket->WeaponComponent = Weapon->GetComponentByClass<UC_WSWeapon>();
				ModifierTicket->WeaponModifier =
					{
					.DamageModifier = 1.2f,
					.AttackSpeed = 1.5f,
					.FixedDamage = 0.f
					};

				WorldStatusSubsystem->PushTicket(ModifierTicket);
			}
		}
	}
}
