// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_WSCharacterSnapshot.h"

#include "wunthshin/Actors/Item/Weapon/A_WSWeapon.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "wunthshin/Components/Weapon/C_WSWeapon.h"

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
	
	if (CharacterContext.WeaponName != NAME_None)
	{
		AA_WSWeapon* Weapon = InWorld->SpawnActorDeferred<AA_WSWeapon>
		(
			AA_WSWeapon::StaticClass(),
			Transform,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		Weapon->SetAssetName(CharacterContext.WeaponName);
		Weapon->FinishSpawning(Transform);
		Weapon->GetComponentByClass<UC_WSWeapon>()->SetRespawn(true);
		Weapon->GetComponentByClass<UC_WSPickUp>()->OnPickUp.Broadcast(Character);
	}
	
	return Character;
}
