#include "CharacterContext.h"

#include "Serialization/ObjectReader.h"
#include "Serialization/ObjectWriter.h"

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
	FObjectWriter Writer(Character, RawData);
	HP = Character->GetStatsComponent()->GetHP();
	CharacterHandle = Character->GetDataTableHandle();
}

void FCharacterContext::ApplyCharacterState(AA_WSCharacter* Character) const
{
	FObjectReader Reader(Character, RawData);
	Character->GetStatsComponent()->SetHP(HP);
}
