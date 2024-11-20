// Fill out your copyright notice in the Description page of Project Settings.


#include "A_LootingBox.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Item/ItemSubsystem.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Data/Items/InventoryPair/InventoryPair.h"


// Sets default values
AA_LootingBox::AA_LootingBox(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AA_LootingBox::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AA_LootingBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AA_LootingBox::ApplyAsset(const FTableRowBase* InRowPointer)
{
	if (!InRowPointer) return;

	auto data = reinterpret_cast<const FLootingBoxTableRow*>(InRowPointer);
	Data = *const_cast<FLootingBoxTableRow*>(data);

	InitializeLootingBox();
}

void AA_LootingBox::InitializeLootingBox()
{
	auto RandomItems = Data.RandomItems;
	auto EssentialItems = Data.EssentialItems;
	auto Subsystem = GetGameInstance()->GetSubsystem<UItemSubsystem>();
	auto ItemDataTable = Subsystem->GetDataTable();
	
	// 필수 아이템 초기화
	for (auto& Item : EssentialItems)
	{
		auto MetaData = Subsystem->GetMetadata(Item.ItemRowHandle.RowName);
		uint64 Quantity = FMath::RandRange((double)Item.MinQuantity, (double)Item.MaxQuantity);
		Items.Emplace(FInventoryPair(MetaData, Quantity));
	}
	
	// 랜덤 아이템 초기화
	TArray<FInventoryPair> ResultArray;
	auto Diversity = FMath::RandRange((double)Data.MinDiversity, (double)Data.MaxDiversity);
	TArray<FLootItem> temp;
	
	for (auto& Item : RandomItems)
	{
		ERarity Rarity = Item.ItemRarity;
		EItemType Type = Item.ItemType;
		uint64 Quantity = FMath::RandRange((double)Item.MinQuantity, (double)Item.MaxQuantity);
		
		// 아이템 테이블을 순회하며 조건 맞는 것만 필터링
		ItemDataTable->ForeachRow<FItemTableRow>(TEXT(""), [&,this](const FName Key, const FItemTableRow TableRow)
		{
			bool bIsCorrect = Rarity == TableRow.ItemRarity && Type == TableRow.ItemType;
			
			if (bIsCorrect)
			{
				auto MetaData = Subsystem->GetMetadata(TableRow.ItemName);
			}
		});
	}
}

void AA_LootingBox::Interaction()
{
	// auto owner = Cast<AA_WSCharacter>(PickUpComponent->GetOwner());
	// if (owner) return;
	//
	// auto inven = owner->GetComponentByClass<UC_WSInventory>();
	//
	// for (auto& Item : Items)
	// {
	// }

	//Destroy();
}
