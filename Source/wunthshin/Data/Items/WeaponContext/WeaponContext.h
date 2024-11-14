#pragma once
#include "wunthshin/Data/Modifiers/WeaponModifier/WeaponModifier.h"

#include "WeaponContext.generated.h"

USTRUCT(BlueprintType)
struct FWeaponContext
{
	GENERATED_BODY()
	
	float GetDamage() const { return (BaseDamage + WeaponModifier.FixedDamage) * WeaponModifier.DamageModifier; }
	float GetAttackSpeed() const { return WeaponModifier.AttackSpeed; }

	void SetBaseDamage(const float Damage) { BaseDamage = Damage; }
	void SetModifier(const FWeaponModifier& Modifier) { WeaponModifier = Modifier; }
	void ResetModifier() { WeaponModifier = {}; }
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float BaseDamage = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	FWeaponModifier WeaponModifier;
};
