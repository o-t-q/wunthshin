// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/WeaponSubsystem.h"

#include "Data/Item/ItemTableRow.h"
#include "Data/Item/SG_WSItemMetadata.h"
#include "Data/Effect/EffectRowHandle.h"
#include "Subsystem/Utility.h"

UWeaponSubsystem::UWeaponSubsystem()
	: DataTable(nullptr) {}

void UWeaponSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_WeaponTable.DT_WeaponTable'")));
	check(DataTable);
	
	FItemSubsystemUtility::UpdateTable<FWeaponTableRow>(DataTable, WeaponMetadata.MetadataByID, WeaponMetadata.Metadata);
	DataTableMapping.Emplace(FWeaponTableRow::StaticStruct(), DataTable);
}

USG_WSItemMetadata* UWeaponSubsystem::GetMetadata(const EItemType InItemType, const FName& InAssetName)
{
	if (InItemType == EItemType::Weapon)
	{
		return FItemSubsystemUtility::GetMetadataTemplate(WeaponMetadata.Metadata, InAssetName);
	}

	check(false);
	return nullptr;
}

USG_WSItemMetadata* UWeaponSubsystem::GetMetadata(const EItemType InItemType, const int32 InID)
{
	if (InItemType == EItemType::Weapon)
	{
		return FItemSubsystemUtility::GetMetadataByIDTemplate(WeaponMetadata.MetadataByID, InID);
	}

	check(false);
	return nullptr;
}
