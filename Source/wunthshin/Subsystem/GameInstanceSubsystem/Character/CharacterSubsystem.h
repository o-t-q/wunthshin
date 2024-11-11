// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "wunthshin/Data/Characters/CharacterContext/CharacterContext.h"
#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "CharacterSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UCharacterSubsystem : public UGameInstanceSubsystem, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* AssetDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* StatDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	int32 CurrentSpawnedIndex = 0;

	TMap<int32, FCharacterContext> PossibleCharacters;

public:
	UCharacterSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void SaveCharacterState(AA_WSCharacter* InCharacter, int32 InIndex);
	
	UFUNCTION()
	void SaveCharacterState();
	void SpawnAsCharacter(const int32 InIndex);
};