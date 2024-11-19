#pragma once
#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"

#include "CharacterContext.generated.h"

class AA_WSCharacter;

USTRUCT(BlueprintType)
struct FCharacterContext
{
	GENERATED_BODY()

	FCharacterContext() = default;
	explicit FCharacterContext(const AA_WSCharacter* InCharacter);
	
	UPROPERTY()
	FName AssetName = NAME_None;

	UPROPERTY()
	FName WeaponName = NAME_None;

	UPROPERTY()
	FCharacterStats CharacterStats;
};
