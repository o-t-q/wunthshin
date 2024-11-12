// Fill out your copyright notice in the Description page of Project Settings.

#ifdef WITH_EDITORONLY_DATA
#include "CharacterEditorSubsystem.h"

#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"

void UCharacterEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	DataTableMapping.Emplace(FCharacterTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FCharacterStats::StaticStruct(), StatDataTable);
}

UCharacterEditorSubsystem::UCharacterEditorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> AssetTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterTable.DT_CharacterTable'"));
	check(AssetTable.Object);
	AssetDataTable = AssetTable.Object;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> StatTable(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterStatsTable.DT_CharacterStatsTable'"));
	check(StatTable.Object);
	StatDataTable = StatTable.Object;
}
#endif