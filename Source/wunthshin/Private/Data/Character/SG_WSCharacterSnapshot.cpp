// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Character/SG_WSCharacterSnapshot.h"

#include "Actor/Item/A_WSWeapon.h"
#include "Actor/Pawn/AA_WSCharacter.h"
#include "Component/C_WSPickUp.h"
#include "Component/StatsComponent.h"
#include "Component/C_WSWeapon.h"

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
