#include "CharacterContext.h"

#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/Stats/StatsComponent.h"

FCharacterContext::FCharacterContext(const AA_WSCharacter* InCharacter)
{
	AssetName = InCharacter->GetAssetName();
	CharacterStats = InCharacter->GetStatsComponent()->GetStats();
}
