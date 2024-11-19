#pragma once
#include "wunthshin/Data/Items/InventoryPair/InventoryPair.h"

#include "SharedInventory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSharedInventory, Log, All);

class USG_WSItemMetadata;

USTRUCT(BlueprintType)
struct FSharedInventory
{
	GENERATED_BODY()

	const TArray<FInventoryPair>& GetItems() const { return ItemsOwned; }
	void AddItem(const USG_WSItemMetadata* InItemMetadata, const uint32 InCount);
	void RemoveItem(const USG_WSItemMetadata* InItemMetadata, const uint32 InCount);
	void UseItem(uint32 InIndex, AActor* InUser, AActor* InTargetActor, uint32 InCount);

private:
	int32 FindItemIndex(const USG_WSItemMetadata* InMetadata) const;
	FInventoryPair* FindItem(const USG_WSItemMetadata* InMetadata);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	TArray<FInventoryPair> ItemsOwned;
};
