// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "CharacterSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterAdded);


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
	TArray<TObjectPtr<AA_WSCharacter>> OwnedCharacters;
	
	TArray<TArray<uint8>> CharacterSnapshots;
	
public:
	UCharacterSubsystem();

	FOnCharacterAdded OnCharacterAdded;
	
	UFUNCTION()
	void TakeCharacterLevelSnapshot();
	UFUNCTION()
	void LoadCharacterLevelSnapshot();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	int32 GetAvailableCharacter() const;

	const TArray<AA_WSCharacter*>& GetOwnedCharacters() const { return OwnedCharacters;}

	AA_WSCharacter* GetCharacter(const int32 InIndex) const
	{
		if (OwnedCharacters.Num() > InIndex)
		{
			return OwnedCharacters[InIndex];
		}

		return nullptr;
	}

	int32 GetIndexOfCharacter(const AA_WSCharacter* InCharacter) const;

	AA_WSCharacter* GetCurrentCharacter() const
	{
		if (OwnedCharacters.Num() > CurrentSpawnedIndex)
		{
			return OwnedCharacters[CurrentSpawnedIndex];
		}

		return nullptr;
	}
	
	void AddCharacter(AA_WSCharacter* InCharacter);
	void AddCharacter(AA_WSCharacter* Character, int32 InIndex);
	void SpawnAsCharacter(const int32 InIndex);
};