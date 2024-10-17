// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSubsystem.h"

UCharacterSubsystem::UCharacterSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterTable.DT_CharacterTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}

UCharacterEditorSubsystem::UCharacterEditorSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterTable.DT_CharacterTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}
