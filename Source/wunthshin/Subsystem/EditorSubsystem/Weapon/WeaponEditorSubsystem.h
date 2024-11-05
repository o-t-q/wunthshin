// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"

#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "wunthshin/Interfaces/ItemMetadataGetter/ItemMetadataGetter.h"

#include "WeaponEditorSubsystem.generated.h"

class USG_WSItemMetadata;

/**
 * 
 */
UCLASS()
class UWeaponEditorSubsystem : public UEditorSubsystem, public IItemMetadataGetter, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	TMap<FName, USG_WSItemMetadata*> Metadata;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* DataTable;
	
public:
	UWeaponEditorSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual USG_WSItemMetadata* GetMetadata(const FName& InAssetName) override;
	virtual bool IsEditorOnly() const override;

};
