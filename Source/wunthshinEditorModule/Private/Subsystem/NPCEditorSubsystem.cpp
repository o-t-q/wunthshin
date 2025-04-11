// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/NPCEditorSubsystem.h"
#include "Data/NPC/NPCTableRow.h"
#include "Data/NPC/NPCStats.h"

void UNPCEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AssetDataTable = CastChecked<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_NPCTable.DT_NPCTable'")));
	StatDataTable = CastChecked<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_NPCStatsTable.DT_NPCStatsTable'")));
	
	DataTableMapping.Emplace(FNPCTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FNPCStats::StaticStruct(), StatDataTable);
}

UNPCEditorSubsystem::UNPCEditorSubsystem(): AssetDataTable(nullptr), StatDataTable(nullptr)
{
}
