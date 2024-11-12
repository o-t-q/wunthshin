#pragma once
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/EventTicket/EventTicket.h"

class UC_WSWeapon;

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponModifier, Log, All);

struct FWeaponModifierTicket : public FEventTicket
{
public:
	UC_WSWeapon* WeaponComponent;
	float DamageModifier = 1.f;
	float AttackSpeed = 1.f;
	float BuffDuration = 5.f;

	FTimerHandle BuffTimerHandle;

	virtual void Execute(UWorld* InWorld) override;
};

struct FWeaponModifierFinishTicket : public FEventTicket
{
public:
	UC_WSWeapon* WeaponComponent;

	virtual void Execute(UWorld* InWorld) override;
};