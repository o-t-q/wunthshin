// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ClientCharacterInfo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterAdded);

class AA_WSCharacter;

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AClientCharacterInfo : public AInfo
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Character", meta = (AllowPrivateAccess = "true"))
	int32 CurrentSpawnedIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Character", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<AA_WSCharacter>> OwnedCharacters;
	
	TArray<TArray<uint8>> CharacterSnapshots;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UPROPERTY(BlueprintAssignable)
	FOnCharacterAdded OnCharacterAdded;

	UFUNCTION()
	void ResetPlayer();

	UFUNCTION()
	void TakeCharacterLevelSnapshot();

	UFUNCTION()
	void LoadCharacterLevelSnapshot();

	UFUNCTION(Client, Reliable)
	void Client_OnRep_OnCharacterAdded();

	int32 GetAvailableCharacter() const;

	int32 GetIndexOfCharacter(const AA_WSCharacter* InCharacter) const;

	void AddCharacter(AA_WSCharacter* InCharacter);

	void AddCharacter(AA_WSCharacter* Character, const int32 InIndex);

	const TArray<AA_WSCharacter*>& GetOwnedCharacters() const;

	AA_WSCharacter* GetCharacter(const int32 InIndex) const;

	bool IsOwnedCharacter(const AA_WSCharacter* InCharacter) const;

	AA_WSCharacter* GetCurrentCharacter() const;

	void SpawnAsCharacter(const int32 InIndex);
};
