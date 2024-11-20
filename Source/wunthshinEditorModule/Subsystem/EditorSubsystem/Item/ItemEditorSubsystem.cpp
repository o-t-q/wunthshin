// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemEditorSubsystem.h"

#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Subsystem/Utility.h"

USG_WSItemMetadata* UItemEditorSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate(Metadata, InAssetName);
}

UItemEditorSubsystem::UItemEditorSubsystem(): DataTable(nullptr)
{
}

void UItemEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = CastChecked<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'")));
	
	FItemSubsystemUtility::UpdateTable<FItemTableRow>(DataTable, Metadata);
	DataTableMapping.Emplace(FItemTableRow::StaticStruct(), DataTable);
}
