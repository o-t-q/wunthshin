#include "CharacterContext.h"

#include "wunthshin/Actors/Item/Weapon/A_WSWeapon.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/Stats/StatsComponent.h"

FCharacterContext::FCharacterContext(const AA_WSCharacter* InCharacter)
{
	AssetName = InCharacter->GetAssetName();

	if (const AA_WSWeapon* Weapon = Cast<AA_WSWeapon>(InCharacter->GetRightHandComponent()->GetChildActor()))
	{
		WeaponName = Weapon->GetAssetName();
	}
	
	CharacterStats = InCharacter->GetStatsComponent()->GetStats();
}
