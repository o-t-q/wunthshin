// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshinPlayerState.h"

#include "Actors/Pawns/Character/AA_WSCharacter.h"
#include "Serialization/ObjectReader.h"
#include "Serialization/ObjectWriter.h"

void AwunthshinPlayerState::SaveCharacterState()
{
	if (const APlayerController* PlayerController = GetPlayerController())
	{
		if (AA_WSCharacter* Character = Cast<AA_WSCharacter>(PlayerController->GetPawn()))
		{
			SaveCharacterState(Character, CurrentSpawnedIndex);
		}
	}
}

void AwunthshinPlayerState::SaveCharacterState(AA_WSCharacter* InCharacter, int32 InIndex)
{
	if (!PossibleCharacters.Contains(InIndex))
	{
		PossibleCharacters.Add(InIndex);
	}

	FObjectWriter Writer(InCharacter, PossibleCharacters[InIndex].RawData);
}

void AwunthshinPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AwunthshinPlayerState::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (APlayerController* PlayerController = GetPlayerController())
	{
		TScriptDelegate<> Delegate;
		Delegate.BindUFunction(this, "SaveCharacterState");
		PlayerController->OnPossessedPawnChanged.Add(Delegate);
	}
}

void AwunthshinPlayerState::SpawnAsCharacter(const int32 InIndex)
{
	if (CurrentSpawnedIndex == InIndex)
	{
		return;
	}

	if (!PossibleCharacters.Contains(InIndex))
	{
		return;
	}
	
	if (APlayerController* PlayerController = GetPlayerController())
	{
		if (AA_WSCharacter* CurrentCharacter = Cast<AA_WSCharacter>(PlayerController->GetPawn()))
		{
			SaveCharacterState(CurrentCharacter, CurrentSpawnedIndex);
		}

		const FTransform& PreviousTransform = GetPawn()->GetTransform();

		AA_WSCharacter* SpawnedEmpty = GetWorld()->SpawnActorDeferred<AA_WSCharacter>(AA_WSCharacter::StaticClass(), FTransform::Identity);
		FObjectReader MemoryReader(SpawnedEmpty, PossibleCharacters[InIndex].RawData, true);
		SpawnedEmpty->FinishSpawning(PreviousTransform);

		PlayerController->Possess(SpawnedEmpty);
		CurrentSpawnedIndex = InIndex;
	}
}
