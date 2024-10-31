// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCSubsystem.h"
#include "wunthshin/Data/NPCs/NPCTableRow/NPCTableRow.h"
#include "wunthshin/Data/NPCs/NPCStats/NPCStats.h"

UNPCSubsystem::UNPCSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> AssetTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_NPCTable.DT_NPCTable'"));
	check(AssetTable.Object);
	AssetDataTable = AssetTable.Object;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> StatTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_NPCStatsTable.DT_NPCStatsTable'"));
	check(StatTable.Object);
	StatDataTable = StatTable.Object;
}

void UNPCSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	DataTableMapping.Emplace(FNPCTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FNPCStats::StaticStruct(), StatDataTable);
}

void UNPCEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	DataTableMapping.Emplace(FNPCTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FNPCStats::StaticStruct(), StatDataTable);
}

UNPCEditorSubsystem::UNPCEditorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> AssetTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_NPCTable.DT_NPCTable'"));
	check(AssetTable.Object);
	AssetDataTable = AssetTable.Object;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> StatTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_NPCStatsTable.DT_NPCStatsTable'"));
	check(StatTable.Object);
	StatDataTable = StatTable.Object;
}
