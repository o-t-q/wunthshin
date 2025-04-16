// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/WeaponEditorSubsystem.h"

#include "Data/Item/SG_WSItemMetadata.h"
#include "Data/Item/ItemTableRow.h"
#include "Subsystem/Utility.h"

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
	
	FItemSubsystemUtility::UpdateTable<FWeaponTableRow>(DataTable, WeaponMetadata.MetadataByID, WeaponMetadata.Metadata);
	DataTableMapping.Emplace(FWeaponTableRow::StaticStruct(), DataTable);
}

USG_WSItemMetadata* UWeaponEditorSubsystem::GetMetadata(const EItemType InItemType, const FName& InAssetName)
{
	if (InItemType == EItemType::Weapon) 
	{
		return FItemSubsystemUtility::GetMetadataTemplate(WeaponMetadata.Metadata, InAssetName);
	}

	check(false);
	return nullptr;
}

USG_WSItemMetadata* UWeaponEditorSubsystem::GetMetadata(const EItemType InItemType, const int32 InID)
{
	if (InItemType == EItemType::Weapon)
	{
		return FItemSubsystemUtility::GetMetadataByIDTemplate(WeaponMetadata.MetadataByID, InID);
	}

	check(false);
	return nullptr;
}
