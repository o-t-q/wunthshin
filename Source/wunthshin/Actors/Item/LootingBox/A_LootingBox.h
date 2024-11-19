// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"
#include "wunthshin/Interfaces/Taker/Taker.h"
#include "wunthshin/Data/Items/LootingBox/LootingBoxTableRow.h"
#include "A_LootingBox.generated.h"

//--------------------------------------------------------------------------------------------------------------------------------------
//
//	AA_LootingBox
//
//	@Feature
//	- 상호작용시 가지고 있는 아이템 또는 아이템들을 시전자에게 전달
//
//
//

class USG_WSItemMetadata;
class UC_WSInventory;
class UC_WSPickUp;

UCLASS()
class WUNTHSHIN_API AA_LootingBox : public AActor, public I_WSTaker, public IDataTableFetcher
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_LootingBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// 물체가 주어짐을 당했을 때 줍는 대상의 callback
	virtual bool Take(UC_WSPickUp* InTakenComponent) override;

	// 조회한 데이터 테이블의 데이터를 상속받은 클래스에서 사용
	virtual void ApplyAsset(const FTableRowBase* InRowPointer) override;

	virtual UClass* GetSubsystemType() const override { return nullptr;};
#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR 
	virtual UClass* GetEditorSubsystemType() const override { return nullptr; };
#endif
	// 상속된 클래스가 주로 사용할 테이블 row 타입
	virtual UScriptStruct* GetTableType() const override { return FLootingBoxTableRow::StaticStruct(); };
	
protected:
	// 랜덤아이템 세팅
	void InitializeLootingBox();
	
	// OnPickup Broadcast 시 실행할 함수
	void Interaction();
	
protected:
	UPROPERTY()
	UC_WSPickUp* PickUpComponent;

	// UPROPERTY()
	// UC_WSInventory* InventoryComponent;
	
protected:
	UPROPERTY()
	TMap<USG_WSItemMetadata*, uint64> Items;

	UPROPERTY()
	FLootingBoxTableRow Data;
};
