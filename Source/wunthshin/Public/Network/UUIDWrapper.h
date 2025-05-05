#pragma once
#include "CoreMinimal.h"
#include "UUIDWrapper.generated.h"

USTRUCT(BlueprintType)
struct FUUIDWrapper
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<uint8> uuid{};

	bool IsValid() const
	{
		return !std::all_of(uuid.begin(), uuid.end(), [](const uint8& b) {return b == (uint8)0; });
	}

	bool operator==(const FUUIDWrapper& Other) const
	{
		return Other.uuid == uuid;
	}
};

inline uint32 GetTypeHash(const FUUIDWrapper& Value)
{
	constexpr size_t UUIDCount = 32;
	uint32 Hash = Value.uuid[ 0 ];
	for (int i = 1; i < UUIDCount; ++i)
	{
		Hash = HashCombineFast( Hash, Value.uuid[ i ] );
	}

	return Hash;
}

template <>
struct std::hash<FUUIDWrapper>
{
	uint32 operator()(const FUUIDWrapper& uuid) const
	{
		return GetTypeHash( uuid );
	}
};