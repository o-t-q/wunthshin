#include "Data/Character/CharacterContext.h"

#include "Actor/Item/A_WSWeapon.h"
#include "Actor/Pawn/AA_WSCharacter.h"
#include "Component/StatsComponent.h"

FCharacterContext::FCharacterContext(const AA_WSCharacter* InCharacter)
{
	AssetName = InCharacter->GetAssetName();

	if (const AA_WSWeapon* Weapon = Cast<AA_WSWeapon>(InCharacter->GetRightHandComponent()->GetChildActor()))
	{
		WeaponName = Weapon->GetAssetName();
	}
	
	CharacterStats = InCharacter->GetStatsComponent()->GetStats();
}
