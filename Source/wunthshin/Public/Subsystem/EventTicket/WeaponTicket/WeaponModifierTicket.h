#pragma once
#include "Data/Modifier/WeaponModifier.h"
#include "Subsystem/EventTicket/EventTicket.h"

class UC_WSWeapon;

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponModifier, Log, All);

struct WUNTHSHIN_API FWeaponModifierTicket : public FEventTicket
{
public:
	UC_WSWeapon* WeaponComponent;
	FWeaponModifier WeaponModifier;
	float BuffDuration = 5.f;

	FTimerHandle BuffTimerHandle;
	virtual void Execute(UWorld* InWorld) override;
};

struct WUNTHSHIN_API FWeaponModifierRevokeTicket : public FEventTicket
{
public:
	UC_WSWeapon* WeaponComponent;

	virtual void Execute(UWorld* InWorld) override;
};