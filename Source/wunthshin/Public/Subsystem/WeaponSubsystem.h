// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Interface/DataTableQuery.h"
#include "Interface/ItemMetadataGetter.h"

#include "WeaponSubsystem.generated.h"

class USG_WSItemMetadata;

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWeaponSubsystem : public UGameInstanceSubsystem, public IItemMetadataGetter, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	FWSMetadataPair WeaponMetadata;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* DataTable;
	
public:
	UWeaponSubsystem();

	virtual void                Initialize(FSubsystemCollectionBase& Collection) override;
	virtual USG_WSItemMetadata* GetMetadata(const EItemType InItemType, const FName& InAssetName) override;
	virtual USG_WSItemMetadata* GetMetadata(const EItemType InItemType, const int32 InID) override;
};
