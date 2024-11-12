#include "CharacterArchive.h"

#include "wunthshin/Interfaces/CommonPawn/CommonPawn.h"

void FCharacterArchive::SerializeCharacter(ICommonPawn* InCommonPawn)
{
	AssetName = InCommonPawn->GetAssetName();
	InCommonPawn->Serialize(*this);
}

FCharacterArchive& FCharacterArchive::operator<<(ICommonPawn* InCommonPawn)
{
	SerializeCharacter(InCommonPawn);
	return *this;
}
