#pragma once

#include "CoreMinimal.h"
#include "ElementRowHandle.generated.h"

// 커스텀 해시가 FDataTableRowHandle에 전역 적용되는 것을 막기 위한 구조체
USTRUCT(BlueprintType)
struct FElementRowHandle
{
	GENERATED_BODY()	
	
public:
	FElementRowHandle() {}
	explicit FElementRowHandle(const FDataTableRowHandle& InHandle) : Handle(InHandle) {}

	FDataTableRowHandle Handle;

	bool operator==(const FElementRowHandle& Other) const
	{
		return Handle == Other.Handle;
	}
};

uint32 GetTypeHash(const FElementRowHandle& InDataTableHandle);