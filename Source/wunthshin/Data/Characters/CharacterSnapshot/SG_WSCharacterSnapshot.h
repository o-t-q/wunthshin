// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterContext/CharacterContext.h"
#include "GameFramework/SaveGame.h"
#include "SG_WSCharacterSnapshot.generated.h"

class AA_WSCharacter;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API USG_WSCharacterSnapshot : public USaveGame
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FCharacterContext CharacterContext;

public:
	void SetCharacter(const FCharacterContext& InContext) { CharacterContext = InContext; }
	AA_WSCharacter* SpawnCharacter(UWorld* InWorld, const FTransform& Transform, AController* Owner) const;
};
