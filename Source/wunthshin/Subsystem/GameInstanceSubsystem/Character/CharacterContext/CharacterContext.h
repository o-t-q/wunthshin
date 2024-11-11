#pragma once

#include "CharacterContext.generated.h"

USTRUCT()
struct FCharacterContext
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> RawData;
};
