// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/NPCSubsystem.h"
#include "Data/NPC/NPCTableRow.h"
#include "Data/NPC/NPCStats.h"

UNPCSubsystem::UNPCSubsystem()
	: AssetDataTable(nullptr),
	  StatDataTable(nullptr) {}

void UNPCSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AssetDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_NPCTable.DT_NPCTable'")));
	check(AssetDataTable);

	StatDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_NPCStatsTable.DT_NPCStatsTable'")));
	check(StatDataTable);
	
	DataTableMapping.Emplace(FNPCTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FNPCStats::StaticStruct(), StatDataTable);
}
