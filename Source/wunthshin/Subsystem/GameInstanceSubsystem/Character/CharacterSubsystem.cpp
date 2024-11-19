// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSubsystem.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "wunthshin/Data/Characters/CharacterSnapshot/SG_WSCharacterSnapshot.h"
#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"

UCharacterSubsystem::UCharacterSubsystem()
	: AssetDataTable(nullptr), StatDataTable(nullptr) {}

void UCharacterSubsystem::TakeCharacterLevelSnapshot()
{
	for (auto& [Index, Character] : OwnedCharacters)
	{
		USG_WSCharacterSnapshot* Snapshot = Cast<USG_WSCharacterSnapshot>(UGameplayStatics::CreateSaveGameObject(USG_WSCharacterSnapshot::StaticClass()));
		Snapshot->SetCharacter(FCharacterContext(Character));

		if (!CharacterSnapshots.Contains(Index))
		{
			CharacterSnapshots.Add(Index);
		}
		
		UGameplayStatics::SaveGameToMemory(Snapshot,CharacterSnapshots[Index]); 
	}
}

void UCharacterSubsystem::LoadCharacterLevelSnapshot()
{
	for (auto& [Index, Character] : CharacterSnapshots)
	{
		if (const USG_WSCharacterSnapshot* Snapshot = Cast<USG_WSCharacterSnapshot>(UGameplayStatics::LoadGameFromMemory(CharacterSnapshots[Index])))
		{
			if (OwnedCharacters[Index] == nullptr)
			{
				OwnedCharacters[Index] = Snapshot->SpawnCharacter(GetWorld(), FTransform::Identity, nullptr);
				OwnedCharacters[Index]->SetActorEnableCollision(false);
				OwnedCharacters[Index]->SetActorHiddenInGame(true);
			}
		}
	}
}

void UCharacterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AssetDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterTable.DT_CharacterTable'")));
	check(AssetDataTable);
	StatDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterStatsTable.DT_CharacterStatsTable'")));
	check(StatDataTable);
	
	DataTableMapping.Emplace(FCharacterTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FCharacterStats::StaticStruct(), StatDataTable);
}

int32 UCharacterSubsystem::GetAvailableCharacter() const
{
	for (const auto& [Index, Character] : OwnedCharacters)
	{
		if (Character->GetStatsComponent()->GetHP() > 0)
		{
			return Index;
		}
	}

	return -1;
}

void UCharacterSubsystem::AddCharacter(AA_WSCharacter* InCharacter)
{
	for (int i = 0; i < 4; ++i)
	{
		if (OwnedCharacters.Contains(i))
		{
			if (OwnedCharacters[i] == InCharacter)
			{
				return;
			}
			
			continue;
		}

		AddCharacter(InCharacter, i);
		return;
	}
}

void UCharacterSubsystem::AddCharacter(AA_WSCharacter* Character, const int32 InIndex)
{
	if (!OwnedCharacters.Contains(InIndex))
	{
		OwnedCharacters.Add(InIndex);
		OwnedCharacters[InIndex] = Character;
	}
}

void UCharacterSubsystem::SpawnAsCharacter(const int32 InIndex)
{
	if (CurrentSpawnedIndex == InIndex)
	{
		return;
	}

	if (!OwnedCharacters.Contains(InIndex))
	{
		return;
	}

	if (OwnedCharacters[InIndex]->GetStatsComponent()->GetHP() < 0)
	{
		return;
	}
	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		AA_WSCharacter* CurrentCharacter = Cast<AA_WSCharacter>(PlayerController->GetPawn());
		const FTransform PreviousTransform = CurrentCharacter->GetActorTransform();
			
		if (!OwnedCharacters.Contains(CurrentSpawnedIndex))
		{
			OwnedCharacters.Add(CurrentSpawnedIndex);
			OwnedCharacters[CurrentSpawnedIndex] = CurrentCharacter;
		}

		CurrentCharacter->GetCharacterMovement()->StopMovementImmediately();
		CurrentCharacter->SetActorEnableCollision(false);
		CurrentCharacter->SetActorHiddenInGame(true);
		CurrentCharacter->SetActorTransform(FTransform::Identity);

		PlayerController->Possess(OwnedCharacters[InIndex]);
		OwnedCharacters[InIndex]->SetActorEnableCollision(true);
		OwnedCharacters[InIndex]->SetActorHiddenInGame(false);
		OwnedCharacters[InIndex]->SetActorTransform(PreviousTransform);
		
		CurrentSpawnedIndex = InIndex;
	}
}
