#pragma once

#include "SkillRowHandle.generated.h"

USTRUCT(BlueprintType)
struct WUNTHSHIN_API FSkillRowHandle
{
	GENERATED_BODY()

	FDataTableRowHandle Handle;

public:
	FSkillRowHandle() : Handle() {}

	explicit FSkillRowHandle(const FDataTableRowHandle& InHandle) : Handle(InHandle) {}
};

uint32 GetTypeHash(const FSkillRowHandle& InDataTableHandle);