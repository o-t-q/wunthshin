// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSubsystem.h"

#include "wunthshin/Data/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Subsystem/Utility.h"

UWeaponSubsystem::UWeaponSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_WeaponTable.DT_WeaponTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

void UWeaponSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FItemSubsystemUtility::UpdateTable<FWeaponTableRow, USG_WSItemMetadata>(DataTable, Metadata);
	DataTableMapping.Emplace(FWeaponTableRow::StaticStruct(), DataTable);
}

USG_WSItemMetadata* UWeaponSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate<USG_WSItemMetadata>(Metadata, InAssetName);
}

USG_WSItemMetadata* UWeaponEditorSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate<USG_WSItemMetadata>(Metadata, InAssetName);
}

UWeaponEditorSubsystem::UWeaponEditorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_WeaponTable.DT_WeaponTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

void UWeaponEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FItemSubsystemUtility::UpdateTable<FWeaponTableRow, USG_WSItemMetadata>(DataTable, Metadata);

	DataTableMapping.Emplace(FWeaponTableRow::StaticStruct(), DataTable);
}
