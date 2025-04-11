// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ItemSubsystem.h"

#include "Data/Item/SG_WSItemMetadata.h"
#include "Data/Effect/EffectRowHandle.h"
#include "Data/Item/ItemTableRow.h"
#include "Data/Item/LootingBoxTableRow.h"
#include "Subsystem/Utility.h"

UItemSubsystem::UItemSubsystem()
	: DataTable(nullptr), LootingBoxTable(nullptr)
{
}

void UItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'")));
	check(DataTable);

	LootingBoxTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_LootingBoxTable.DT_LootingBoxTable'")));
	check(LootingBoxTable);
	
	FItemSubsystemUtility::UpdateTable<FItemTableRow>(DataTable, Metadata);
	FItemSubsystemUtility::UpdateTable<FLootingBoxTableRow>(LootingBoxTable, Metadata, false);
	DataTableMapping.Emplace(FItemTableRow::StaticStruct(), DataTable);
	DataTableMapping.Emplace(FLootingBoxTableRow::StaticStruct(), LootingBoxTable);
}

USG_WSItemMetadata* UItemSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate(Metadata, InAssetName);
}

FSharedInventory& UItemSubsystem::GetSharedInventory() 
{ 
	return SharedInventory; 
}
