// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSubsystem.h"

#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"

UCharacterSubsystem::UCharacterSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> AssetTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterTable.DT_CharacterTable'"));
	check(AssetTable.Object);
	AssetDataTable = AssetTable.Object;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> StatTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterStatsTable.DT_CharacterStatsTable'"));
	check(StatTable.Object);
	StatDataTable = StatTable.Object;
}

void UCharacterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	DataTableMapping.Emplace(FCharacterTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FCharacterStats::StaticStruct(), StatDataTable);
}

void UCharacterSubsystem::SaveCharacterState()
{
	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AA_WSCharacter* Character = Cast<AA_WSCharacter>(PlayerController->GetPawn()))
		{
			SaveCharacterState(Character, CurrentSpawnedIndex);
		}
	}
}

void UCharacterSubsystem::SaveCharacterState(AA_WSCharacter* InCharacter, const int32 InIndex)
{
	if (!PossibleCharacters.Contains(InIndex))
	{
		PossibleCharacters.Add(InIndex);
	}

	PossibleCharacters[InIndex].SaveCharacterState(InCharacter);
}

void UCharacterSubsystem::SpawnAsCharacter(const int32 InIndex)
{
	if (CurrentSpawnedIndex == InIndex)
	{
		return;
	}

	if (!PossibleCharacters.Contains(InIndex))
	{
		return;
	}
	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		const FTransform& PreviousTransform = GetWorld()->GetFirstPlayerController()->GetPawn()->GetTransform();

		AA_WSCharacter* CurrentCharacter = Cast<AA_WSCharacter>(PlayerController->GetPawn());
		SaveCharacterState(CurrentCharacter, CurrentSpawnedIndex);

		AA_WSCharacter* SpawnedEmpty = GetWorld()->SpawnActorDeferred<AA_WSCharacter>(AA_WSCharacter::StaticClass(), FTransform::Identity);
		PossibleCharacters[InIndex].ApplyCharacterState(SpawnedEmpty);
		SpawnedEmpty->FinishSpawning(PreviousTransform);

		PlayerController->UnPossess();
		CurrentCharacter->Destroy();
		PlayerController->Possess(SpawnedEmpty);
		
		CurrentSpawnedIndex = InIndex;
	}
}
