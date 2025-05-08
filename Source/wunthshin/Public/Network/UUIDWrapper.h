#pragma once
#include <message.h>

#include "CoreMinimal.h"
#include "UUIDWrapper.generated.h"

USTRUCT(BlueprintType)
struct FUUIDWrapper
{
	GENERATED_BODY()

	FUUIDWrapper() = default;
	
	FUUIDWrapper( const UUID& InStdUUID )
	{
		for (int i = 0; i < InStdUUID.size(); i++)
		{
			uuid.Add( static_cast<uint8>( InStdUUID[ i ] ) );
		}
	}

	UPROPERTY(VisibleAnywhere)
	TArray<uint8> uuid{};

	operator UUID() const
	{
		UUID Result{};
		check (uuid.Num() == Result.size());
		
		for (size_t i = 0; i < uuid.Num(); i++)
		{
			Result[i] = static_cast<std::byte>( uuid[ i ] );
		}
		return Result;
	}
	
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
	constexpr size_t UUIDCount = 16;
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