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
	
	FItemSubsystemUtility::UpdateTable<FWeaponTableRow>(DataTable, Metadata);
	DataTableMapping.Emplace(FWeaponTableRow::StaticStruct(), DataTable);
}

USG_WSItemMetadata* UWeaponSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate(Metadata, InAssetName);
}
