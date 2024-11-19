#pragma once
#include "wunthshin/Data/Modifiers/WeaponModifier/WeaponModifier.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/EventTicket/EventTicket.h"

class UC_WSWeapon;

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponModifier, Log, All);

struct FWeaponModifierTicket : public FEventTicket
{
public:
	UC_WSWeapon* WeaponComponent;
	FWeaponModifier WeaponModifier;
	float BuffDuration = 5.f;

	FTimerHandle BuffTimerHandle;
	virtual void Execute(UWorld* InWorld) override;
};

struct FWeaponModifierRevokeTicket : public FEventTicket
{
public:
	UC_WSWeapon* WeaponComponent;

	virtual void Execute(UWorld* InWorld) override;
};