#pragma once
#include "Data/Item/InventoryPair.h"

#include "SharedInventory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSharedInventory, Log, All);

class USG_WSItemMetadata;

USTRUCT(BlueprintType)
struct WUNTHSHIN_API FSharedInventory
{
	GENERATED_BODY()

	const TArray<FInventoryPair>& GetItems() const;
	void AddItem(const USG_WSItemMetadata* InItemMetadata, const uint32 InCount);
	void RemoveItem(const USG_WSItemMetadata* InItemMetadata, const uint32 InCount);
	void UseItem(uint32 InIndex, AActor* InUser, AActor* InTargetActor, uint32 InCount);

	int32 FindItemIndex(const USG_WSItemMetadata* InMetadata) const;
	FInventoryPair* FindItem(const USG_WSItemMetadata* InMetadata);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	TArray<FInventoryPair> ItemsOwned;
};
