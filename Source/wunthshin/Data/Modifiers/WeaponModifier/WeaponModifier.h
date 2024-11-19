#pragma once

#include "WeaponModifier.generated.h"

USTRUCT(BlueprintType)
struct FWeaponModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifier", meta = (AllowPrivateAccess = "true"))
	float DamageModifier = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifier", meta = (AllowPrivateAccess = "true"))
	float AttackSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifier", meta = (AllowPrivateAccess = "true"))
	float FixedDamage = 0.f;
};
