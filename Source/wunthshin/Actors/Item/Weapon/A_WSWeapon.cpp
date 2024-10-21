// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSWeapon.h"

#include "Components/ShapeComponent.h"

#include "Engine/DataTable.h"
#include "wunthshin/Components/Weapon/C_WSWeapon.h"
#include "wunthshin/Data/ItemTableRow.h"

AA_WSWeapon::AA_WSWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WeaponComponent = CreateDefaultSubobject<UC_WSWeapon>(TEXT("Weapon"));
}

UScriptStruct* AA_WSWeapon::GetTableType() const
{
	return FWeaponTableRow::StaticStruct();
}

void AA_WSWeapon::ApplyAsset(const FDataTableRowHandle& InRowHandle)
{
	// Item 데이터 테이블과 무기 데이터 테이블은 따로 있기 때문에
	// 슈퍼 클래스 호출은 무시
	
	if (InRowHandle.IsNull())
	{
		return;
	}

	const FWeaponTableRow* TableRow = InRowHandle.GetRow<FWeaponTableRow>(TEXT(""));

	if (!TableRow)
	{
		return;
	}
	
	if (TableRow->StaticMesh)
	{
		GetMesh()->SetStaticMesh(TableRow->StaticMesh);
	}

	if (TableRow->CollisionShape)
	{
		UpdateCollisionFromDataTable(TableRow);
	}
	
	if (TableRow->GlobalItemMetadataPointer) 
	{
		ItemMetadata = TableRow->GlobalItemMetadataPointer;
	}

	DefaultAttackMontage = TableRow->DefaultAttackMontage;
}
