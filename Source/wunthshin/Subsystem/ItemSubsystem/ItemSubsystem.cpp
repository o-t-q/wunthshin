// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSubsystem.h"

#include "wunthshin/Data/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Data/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Subsystem/Utility.h"

UItemSubsystem::UItemSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

void UItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FItemSubsystemUtility::UpdateTable<FItemTableRow, USG_WSItemMetadata>(DataTable, Metadata);
}

USG_WSItemMetadata* UItemSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate<USG_WSItemMetadata>(Metadata, InAssetName);
}

USG_WSItemMetadata* UItemEditorSubsystem::GetMetadata(const FName& InAssetName)
{
	return FItemSubsystemUtility::GetMetadataTemplate<USG_WSItemMetadata>(Metadata, InAssetName);
}

UItemEditorSubsystem::UItemEditorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

void UItemEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FItemSubsystemUtility::UpdateTable<FItemTableRow, USG_WSItemMetadata>(DataTable, Metadata);
}
