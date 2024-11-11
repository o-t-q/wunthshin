// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Subsystem/GameInstanceSubsystem/Character/CharacterContext/CharacterContext.h"
#include "wunthshinPlayerState.generated.h"

class AA_WSCharacter;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AwunthshinPlayerState : public APlayerState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	int32 CurrentSpawnedIndex = 0;

	TMap<int32, FCharacterContext> PossibleCharacters;
	
	void SaveCharacterState(AA_WSCharacter* InCharacter, int32 InIndex);

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UFUNCTION()
	void SaveCharacterState();
	void SpawnAsCharacter(const int32 InIndex);
};
