// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "GenericPlatform/GenericPlatformMisc.h"

#include "Interface/DataTableQuery.h"
#include "Interface/ItemMetadataGetter.h"
#include "Subsystem/WSItemSubsystem.h"

#include "ItemEditorSubsystem.generated.h"

class USG_WSItemMetadata;

/**
 * 
 */
UCLASS()
class WUNTHSHINEDITORMODULE_API UItemEditorSubsystem : public UEditorSubsystem, public IItemMetadataGetter, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	FWSMetadataPair ItemMetadata;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	FWSMetadataPair LootingBoxMetadata;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* DataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* LootingBoxTable;
public:
	UItemEditorSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual USG_WSItemMetadata* GetMetadata(const EItemType InItemType, const FName& InAssetName) override;
	virtual USG_WSItemMetadata* GetMetadata(const EItemType InItemType, const int32 InID) override;
	virtual bool IsEditorOnly() const override { return true; }
};