// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "SharedInventory/SharedInventory.h"

#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "wunthshin/Interfaces/ItemMetadataGetter/ItemMetadataGetter.h"
#include "ItemSubsystem.generated.h"

class USG_WSItemMetadata;

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UItemSubsystem : public UGameInstanceSubsystem, public IItemMetadataGetter, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	TMap<FName, USG_WSItemMetadata*> Metadata;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	FSharedInventory SharedInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* DataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* LootingBoxTable;

public:
	UItemSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual USG_WSItemMetadata* GetMetadata(const FName& InAssetName) override;
	FSharedInventory& GetSharedInventory();
	UDataTable* GetDataTable() const { return DataTable; }
};
