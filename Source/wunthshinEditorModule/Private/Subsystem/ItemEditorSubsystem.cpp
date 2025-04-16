// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ItemEditorSubsystem.h"

#include "Data/Item/SG_WSItemMetadata.h"
#include "Data/Item/ItemTableRow.h"
#include "Data/Item/LootingBoxTableRow.h"
#include "Subsystem/Utility.h"

UItemEditorSubsystem::UItemEditorSubsystem()
	: DataTable(nullptr),
	  LootingBoxTable(nullptr) {}

void UItemEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = CastChecked<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'")));
	LootingBoxTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_LootingBoxTable.DT_LootingBoxTable'")));
	check(LootingBoxTable);
	
	FItemSubsystemUtility::UpdateTable<FItemTableRow>(DataTable, ItemMetadata.MetadataByID, ItemMetadata.Metadata);
	FItemSubsystemUtility::UpdateTable<FLootingBoxTableRow>(LootingBoxTable, LootingBoxMetadata.MetadataByID, LootingBoxMetadata.Metadata);
	DataTableMapping.Emplace(FLootingBoxTableRow::StaticStruct(), LootingBoxTable);
	DataTableMapping.Emplace(FItemTableRow::StaticStruct(), DataTable);
}

USG_WSItemMetadata* UItemEditorSubsystem::GetMetadata(const EItemType InItemType, const FName& InAssetName)
{
	switch (InItemType)
	{
	case EItemType::Consumable:
		return FItemSubsystemUtility::GetMetadataTemplate(ItemMetadata.Metadata, InAssetName);
	case EItemType::LootingBox:
		return FItemSubsystemUtility::GetMetadataTemplate(LootingBoxMetadata.Metadata, InAssetName);
	default: check(false);
	}

	return nullptr;
}

USG_WSItemMetadata* UItemEditorSubsystem::GetMetadata(const EItemType InItemType, const int32 InID)
{
	switch (InItemType)
	{
	case EItemType::Consumable:
		return FItemSubsystemUtility::GetMetadataByIDTemplate(ItemMetadata.MetadataByID, InID);
	case EItemType::LootingBox:
		return FItemSubsystemUtility::GetMetadataByIDTemplate(LootingBoxMetadata.MetadataByID, InID);
	default: check(false);
	}

	return nullptr;
}
