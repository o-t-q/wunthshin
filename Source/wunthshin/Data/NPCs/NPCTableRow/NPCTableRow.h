#pragma once

#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"

#include "NPCTableRow.generated.h"

USTRUCT()
struct WUNTHSHIN_API FNPCTableRow : public FCharacterTableRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnemy;

	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/wunthshin.CharacterStats"))
	FDataTableRowHandle Stat;

};