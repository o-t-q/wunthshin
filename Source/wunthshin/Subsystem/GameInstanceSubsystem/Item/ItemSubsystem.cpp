// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSubsystem.h"

#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Data/Effects/EffectRowHandle/EffectRowHandle.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Subsystem/Utility.h"

UItemSubsystem::UItemSubsystem()
	: DataTable(nullptr) {}

void UItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'")));
	check(DataTable);
	
	FItemSubsystemUtility::UpdateTable<FItemTableRow>(DataTable, Metadata);
	DataTableMapping.Emplace(FItemTableRow::StaticStruct(), DataTable);
}

USG_WSItemMetadata* UItemSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate(Metadata, InAssetName);
}
