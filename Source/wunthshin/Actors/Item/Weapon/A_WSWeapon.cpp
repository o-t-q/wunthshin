// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSWeapon.h"

#include "Engine/DataTable.h"
#include "wunthshin/Components/Weapon/C_WSWeapon.h"


AA_WSWeapon::AA_WSWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WeaponComponent = CreateDefaultSubobject<UC_WSWeapon>(TEXT("Weapon"));
}

void AA_WSWeapon::SetData(const FDataTableRowHandle& InRowHandle)
{
	Super::SetData(InRowHandle);

	if (InRowHandle.IsNull())
	{
		return;
	}

	const TRowTableType* TableRow = InRowHandle.GetRow<TRowTableType>(TEXT(""));

	if (!TableRow)
	{
		return;
	}
	
	DefaultAttackMontage = TableRow->DefaultAttackMontage;
}
