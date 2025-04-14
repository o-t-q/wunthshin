#pragma once
#include "Data/Character/CharacterStats.h"

#include "CharacterContext.generated.h"

class AA_WSCharacter;

USTRUCT(BlueprintType)
struct WUNTHSHIN_API FCharacterContext
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
