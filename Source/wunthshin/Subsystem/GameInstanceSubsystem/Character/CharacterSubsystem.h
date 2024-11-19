// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "CharacterSubsystem.generated.h"

class AA_WSCharacter;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	TMap<int32, TObjectPtr<AA_WSCharacter>> OwnedCharacters;
	
	TMap<int32, TArray<uint8>> CharacterSnapshots;
	
public:
	UCharacterSubsystem();
	
	UFUNCTION()
	void TakeCharacterLevelSnapshot();
	UFUNCTION()
	void LoadCharacterLevelSnapshot();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	int32 GetAvailableCharacter() const;

	AA_WSCharacter* GetCharacter(const int32 InIndex) const
	{
		if (OwnedCharacters.Contains(InIndex))
		{
			return OwnedCharacters[InIndex];
		}

		return nullptr;
	}
	
	AA_WSCharacter* GetCurrentCharacter() const
	{
		if (OwnedCharacters.Contains(CurrentSpawnedIndex))
		{
			return OwnedCharacters[CurrentSpawnedIndex];
		}

		return nullptr;
	}
	
	void AddCharacter(AA_WSCharacter* InCharacter);
	void AddCharacter(AA_WSCharacter* Character, int32 InIndex);
	void SpawnAsCharacter(const int32 InIndex);
};