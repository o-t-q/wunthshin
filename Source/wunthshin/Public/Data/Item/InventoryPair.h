#pragma once

#include "InventoryPair.generated.h"

class USG_WSItemMetadata;
class UInventoryEntryData;

USTRUCT(BlueprintType)
struct WUNTHSHIN_API FInventoryPair
{
	GENERATED_BODY()

	friend UInventoryEntryData;
public:
	FInventoryPair() : Metadata(nullptr), Count(0) {}

	FInventoryPair(const USG_WSItemMetadata* InMetadata) :
		Metadata(InMetadata), Count(1) {}

	FInventoryPair(const USG_WSItemMetadata* InMetadata, const uint64 InCount) :
		Metadata(InMetadata), Count(InCount) {}
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<const USG_WSItemMetadata> Metadata;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	uint32 Count;
	
	bool operator==(const FInventoryPair& InOther) const
	{
		// 테이블로부터 생성된 포인터 비교, unique하다 가정
		return Metadata == InOther.Metadata && 
			Count == InOther.Count;
	}
};