// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemEditorSubsystem.h"

#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Data/Items/LootingBox/LootingBoxTableRow.h"
#include "wunthshin/Subsystem/Utility.h"

USG_WSItemMetadata* UItemEditorSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate(Metadata, InAssetName);
}

UItemEditorSubsystem::UItemEditorSubsystem()
{
}

void UItemEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'")));
	check(DataTable);

	LootingBoxTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_LootingBoxTable.DT_LootingBoxTable'")));
	check(LootingBoxTable);
	
	FItemSubsystemUtility::UpdateTable<FItemTableRow>(DataTable, Metadata);
	FItemSubsystemUtility::UpdateTable<FLootingBoxTableRow>(LootingBoxTable, Metadata, false);
	DataTableMapping.Emplace(FItemTableRow::StaticStruct(), DataTable);
	DataTableMapping.Emplace(FLootingBoxTableRow::StaticStruct(), LootingBoxTable);
}
