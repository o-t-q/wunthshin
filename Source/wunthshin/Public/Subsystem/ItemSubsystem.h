// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "SharedInventory/SharedInventory.h"

#include "Interface/DataTableQuery.h"
#include "Interface/ItemMetadataGetter.h"
#include "ItemSubsystem.generated.h"

enum class EItemType : uint8_t;
class USG_WSItemMetadata;
struct FItemAndCountUE;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInventoryFetched);

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UItemSubsystem : public UGameInstanceSubsystem, public IItemMetadataGetter, public IDataTableQuery
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCharacterInventoryUpdated OnCharacterInventoryUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterInventoryFetched OnCharacterInventoryFetched;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	FWSMetadataPair ItemMetadata;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	FWSMetadataPair LootingBoxMetadata;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	FSharedInventory SharedInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* DataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* LootingBoxTable;

public:
	UItemSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual USG_WSItemMetadata* GetMetadata(const EItemType InItemType, const FName& InAssetName) override;
	virtual USG_WSItemMetadata* GetMetadata(const EItemType InItemType, const int32 InID) override;
	FSharedInventory& GetSharedInventory();
	UDataTable* GetDataTable() const { return DataTable; }

private:
	UFUNCTION()
	void AddItemFromMessage(const EItemType ItemType, const int32 InID, const int32 InCount);
	
	UFUNCTION()
	void UpdateInventory(const bool IsEnd, const int32 Page, const int32 Count, const TArray<FItemAndCountUE>& InItems);

	UFUNCTION()
	void SubscribeServerSubsystemLazy();
};
