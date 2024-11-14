// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_WSCharacterSnapshot.h"

#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/Stats/StatsComponent.h"

AA_WSCharacter* USG_WSCharacterSnapshot::SpawnCharacter(UWorld* InWorld, const FTransform& Transform, AController* Owner) const
{
	AA_WSCharacter* Character = InWorld->SpawnActorDeferred<AA_WSCharacter>
	(
		AA_WSCharacter::StaticClass(),
		Transform,
		Owner,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	Character->SetAssetName(CharacterContext.AssetName);
	Character->GetStatsComponent()->CopyStats(CharacterContext.CharacterStats);
	Character->FinishSpawning(Transform);
	return Character;
}
