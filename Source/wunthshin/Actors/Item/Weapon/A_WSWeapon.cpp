// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSWeapon.h"

#include "Engine/DataTable.h"
#include "wunthshin/Components/Weapon/C_WSWeapon.h"
#include "wunthshin/Data/ItemTableRow.h"

AA_WSWeapon::AA_WSWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WeaponComponent = CreateDefaultSubobject<UC_WSWeapon>(TEXT("Weapon"));
}

void AA_WSWeapon::ApplyAsset(const FDataTableRowHandle& InRowHandle)
{
	Super::ApplyAsset(InRowHandle);

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
