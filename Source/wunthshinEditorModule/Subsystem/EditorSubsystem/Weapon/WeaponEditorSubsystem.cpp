// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponEditorSubsystem.h"

#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Subsystem/Utility.h"

USG_WSItemMetadata* UWeaponEditorSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate(Metadata, InAssetName);
}

bool UWeaponEditorSubsystem::IsEditorOnly() const
{
	return true;
}

UWeaponEditorSubsystem::UWeaponEditorSubsystem(): DataTable(nullptr)
{
}

void UWeaponEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	DataTable = CastChecked<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_WeaponTable.DT_WeaponTable'")));
	
	FItemSubsystemUtility::UpdateTable<FWeaponTableRow>(DataTable, Metadata);
	DataTableMapping.Emplace(FWeaponTableRow::StaticStruct(), DataTable);
}
