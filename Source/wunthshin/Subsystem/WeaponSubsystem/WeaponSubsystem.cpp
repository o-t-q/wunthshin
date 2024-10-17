// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSubsystem.h"

UWeaponSubsystem::UWeaponSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_WeaponTable.DT_WeaponTable'"));
	check(Table.Object);
	DataTable = Table.Object;
}
