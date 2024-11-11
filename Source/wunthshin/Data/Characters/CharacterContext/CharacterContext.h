#pragma once

#include "CharacterContext.generated.h"

class AA_WSCharacter;

USTRUCT()
struct FCharacterContext
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, meta = (RowType = "/Script/wunthshin.CharacterTableRow"))
	FDataTableRowHandle CharacterHandle;
	
	UPROPERTY(VisibleAnywhere)
	float HP = 0.f;
	
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> RawData;

public:
	FName GetCharacterName() const;
	float GetHP() const { return HP; }
	void  SetHP(float InHP);
	
	void SaveCharacterState(AA_WSCharacter* Character);
	void ApplyCharacterState(AA_WSCharacter* Character) const;
};
