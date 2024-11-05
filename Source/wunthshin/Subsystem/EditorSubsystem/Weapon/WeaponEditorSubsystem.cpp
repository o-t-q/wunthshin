// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponEditorSubsystem.h"

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

UWeaponEditorSubsystem::UWeaponEditorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_WeaponTable.DT_WeaponTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

void UWeaponEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FItemSubsystemUtility::UpdateTable<FWeaponTableRow>(DataTable, Metadata);

	DataTableMapping.Emplace(FWeaponTableRow::StaticStruct(), DataTable);
}
