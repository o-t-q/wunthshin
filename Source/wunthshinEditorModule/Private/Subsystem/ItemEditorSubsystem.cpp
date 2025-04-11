// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ItemEditorSubsystem.h"

#include "Data/Item/SG_WSItemMetadata.h"
#include "Data/Item/ItemTableRow.h"
#include "Data/Item/LootingBoxTableRow.h"
#include "Subsystem/Utility.h"

USG_WSItemMetadata* UItemEditorSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate(Metadata, InAssetName);
}

UItemEditorSubsystem::UItemEditorSubsystem()
	: DataTable(nullptr),
	  LootingBoxTable(nullptr) {}

void UItemEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = CastChecked<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'")));
	LootingBoxTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_LootingBoxTable.DT_LootingBoxTable'")));
	check(LootingBoxTable);
	
	FItemSubsystemUtility::UpdateTable<FItemTableRow>(DataTable, Metadata);
	FItemSubsystemUtility::UpdateTable<FLootingBoxTableRow>(LootingBoxTable, Metadata, false);
	DataTableMapping.Emplace(FLootingBoxTableRow::StaticStruct(), LootingBoxTable);
	DataTableMapping.Emplace(FItemTableRow::StaticStruct(), DataTable);
}
