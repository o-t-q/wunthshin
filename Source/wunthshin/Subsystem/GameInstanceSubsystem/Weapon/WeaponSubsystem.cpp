// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSubsystem.h"

#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
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
	FItemSubsystemUtility::UpdateTable<FWeaponTableRow>(DataTable, Metadata);
	DataTableMapping.Emplace(FWeaponTableRow::StaticStruct(), DataTable);
}

USG_WSItemMetadata* UWeaponSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate(Metadata, InAssetName);
}
