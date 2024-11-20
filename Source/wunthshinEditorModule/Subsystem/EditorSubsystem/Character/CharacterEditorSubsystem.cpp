// Fill out your copyright notice in the Description page of Project Settings.

#ifdef WITH_EDITORONLY_DATA
#include "CharacterEditorSubsystem.h"

#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"

void UCharacterEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AssetDataTable = CastChecked<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterTable.DT_CharacterTable'")));
	StatDataTable = CastChecked<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterStatsTable.DT_CharacterStatsTable'")));
	
	check(StatDataTable);
	
	DataTableMapping.Emplace(FCharacterTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FCharacterStats::StaticStruct(), StatDataTable);
}

UCharacterEditorSubsystem::UCharacterEditorSubsystem(): AssetDataTable(nullptr), StatDataTable(nullptr)
{
}
#endif
