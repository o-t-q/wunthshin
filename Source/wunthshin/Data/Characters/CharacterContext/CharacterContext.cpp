#include "CharacterContext.h"

#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Serialization/ObjectReader.h"
#include "Serialization/ObjectWriter.h"
#include "UObject/PropertyProxyArchive.h"

#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"

FName FCharacterContext::GetCharacterName() const
{
	const FCharacterTableRow* TableRow = CharacterHandle.GetRow<FCharacterTableRow>("");
	ensure(TableRow);
	return TableRow->CharacterName;
}

void FCharacterContext::SetHP(const float InHP)
{
	const FCharacterTableRow* TableRow = CharacterHandle.GetRow<FCharacterTableRow>("");
	check(TableRow);
	const FCharacterStats* Stats = TableRow->Stats.GetRow<FCharacterStats>("");
	check(Stats);
	
	HP = FMath::Clamp(InHP, 0, Stats->MaxHP);
}

void FCharacterContext::SaveCharacterState(AA_WSCharacter* Character)
{
	FMemoryWriter Writer(RawData, true);
	FCharacterArchive Ar(Writer);
	Character->Serialize(Ar);
	HP = Character->GetStatsComponent()->GetHP();
	CharacterHandle = Character->GetDataTableHandle();
}

void FCharacterContext::ReloadCharacterState(AA_WSCharacter* Character) const
{
	FMemoryReader Reader(RawData, true);
	FCharacterArchive Ar(Reader);
	Character->Serialize(Ar);
	Character->GetStatsComponent()->SetHP(HP);
}
