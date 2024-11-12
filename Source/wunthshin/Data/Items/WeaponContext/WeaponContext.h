#pragma once
#include "wunthshin/Data/SerializeStruct/WSSerializeStruct.h"

#include "WeaponContext.generated.h"

USTRUCT(BlueprintType)
struct FWeaponContext : public FWSSerializeStruct
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float DamageModifier = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float AttackSpeed = 1.f;
};
