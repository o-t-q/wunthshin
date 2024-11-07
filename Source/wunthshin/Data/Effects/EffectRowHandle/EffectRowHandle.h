#pragma once

#include "EffectRowHandle.generated.h"

USTRUCT()
struct WUNTHSHIN_API FEffectRowHandle
{
	GENERATED_BODY()

	FDataTableRowHandle Handle;

public:
	FEffectRowHandle() : Handle() {}

	explicit FEffectRowHandle(const FDataTableRowHandle& InHandle) : Handle(InHandle) {}
};

uint32 GetTypeHash(const FEffectRowHandle& InDataTableHandle);