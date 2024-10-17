// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSubsystem.h"

UItemSubsystem::UItemSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}
