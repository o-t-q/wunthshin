#pragma once

#include "Enums.h"

#include "EffectTableRow.generated.h"

class UO_WSBaseEffect;

USTRUCT()
struct WUNTHSHIN_API FEffectTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName EffectName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UO_WSBaseEffect> ItemEffect;
};
