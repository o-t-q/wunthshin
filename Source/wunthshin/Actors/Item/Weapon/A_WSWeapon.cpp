// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSWeapon.h"

#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"
#include "wunthshin/Components/Weapon/C_WSWeapon.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"

#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR
#include "wunthshinEditorModule/Subsystem/EditorSubsystem/Weapon/WeaponEditorSubsystem.h"
#endif
#include "wunthshin/Subsystem/GameInstanceSubsystem/Weapon/WeaponSubsystem.h"

AA_WSWeapon::AA_WSWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject("CollisionComponent") // 무기를 손잡이에서 잡기위해 매시 원점 사용
							 .DoNotCreateDefaultSubobject("ItemPhysics")) // 무기를 드랍할 일이 있다면
{
	WeaponComponent = CreateDefaultSubobject<UC_WSWeapon>(TEXT("Weapon"));

	SetRootComponent(GetMesh());
	GetMesh()->SetCollisionProfileName("ItemProfile");
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetItemNotifyWidget()->SetupAttachment(GetMesh());
}

UScriptStruct* AA_WSWeapon::GetTableType() const
{
	return FWeaponTableRow::StaticStruct();
}

UClass* AA_WSWeapon::GetSubsystemType() const
{
	return UWeaponSubsystem::StaticClass();
}

#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR
UClass* AA_WSWeapon::GetEditorSubsystemType() const
{
	return UWeaponEditorSubsystem::StaticClass();
}
#endif

void AA_WSWeapon::ApplyAsset(const FTableRowBase* InRowPointer)
{
	Super::ApplyAsset(InRowPointer);
	
	if (!InRowPointer)
	{
		return;
	}

	const FWeaponTableRow* TableRow = reinterpret_cast<const FWeaponTableRow*>(InRowPointer);
	
	AttackMontages = TableRow->AttackMontages;
	WeaponComponent->GetWeaponContext().SetBaseDamage(TableRow->Damage);
	WeaponElement = FElementRowHandle(TableRow->Element);
}
