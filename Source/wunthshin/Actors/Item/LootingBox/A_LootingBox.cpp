// Fill out your copyright notice in the Description page of Project Settings.


#include "A_LootingBox.h"

#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Item/ItemSubsystem.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Data/Items/InventoryPair/InventoryPair.h"
#include "wunthshin/Subsystem/Utility.h"


// Sets default values
AA_LootingBox::AA_LootingBox(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InventoryComponent = CreateDefaultSubobject<UC_WSInventory>(TEXT("Inventory"));
}

// Called when the game starts or when spawned
void AA_LootingBox::BeginPlay()
{
	Super::BeginPlay();
}

void AA_LootingBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	FetchAsset(DataHandle.RowName);
}

// Called every frame
void AA_LootingBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AA_LootingBox::ApplyAsset(const FTableRowBase* InRowPointer)
{
	if (!InRowPointer) return;

	Super::ApplyAsset(InRowPointer);
	
	auto data = reinterpret_cast<const FLootingBoxTableRow*>(InRowPointer);
	Data = *const_cast<FLootingBoxTableRow*>(data);

	InitializeLootingBox();
}

void AA_LootingBox::InitializeLootingBox()
{
	// 상자 내부 아이템은 게임이 실행될때 결정되어야 함
	if (GetWorld()->IsGameWorld() || GetWorld()->IsPlayInEditor())
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
			InventoryComponent->AddItem(FInventoryPair(MetaData, Quantity));
		}
	
		// 랜덤 아이템 초기화
		TArray<FInventoryPair> ResultArray;
		TArray<FInventoryPair> FilteredItem;
		int Count = 0;
		int32 Diversity = FMath::RandRange((int32)Data.MinDiversity, (int32)Data.MaxDiversity);
	
		while(Count < Diversity)
		{
			uint64 Index = FMath::RandRange(0, RandomItems.Num() - 1);
			FLootItem Item = RandomItems[Index];
			ERarity Rarity = Item.ItemRarity;
			EItemType Type = Item.ItemType;
			uint64 Quantity = FMath::RandRange((double)Item.MinQuantity, (double)Item.MaxQuantity);
		
			// 아이템 테이블을 순회하며 조건 맞는 것만 필터링
			ItemDataTable->ForeachRow<FItemTableRow>(TEXT(""), [&,this](const FName Key, const FItemTableRow TableRow)
			{
				// bool bIsCorrect = Rarity == TableRow.ItemRarity && Type == TableRow.ItemType;
				//
				// if (bIsCorrect)
				// {
				// 	FilteredItem.Add(FInventoryPair(Subsystem->GetMetadata(TableRow.ItemName), Quantity));
				// }
				if(Type == TableRow.ItemType && Rarity == TableRow.ItemRarity)
					FilteredItem.Add(FInventoryPair(Subsystem->GetMetadata(TableRow.ItemName),Quantity));
			});

			// 필터링된 아이템 중 하나를 선정
			uint64 RandomIndex = FMath::RandRange(0, FilteredItem.Num() - 1);
			ResultArray.Add(FilteredItem[RandomIndex]);
			++Count;
		}

		for (const FInventoryPair& Item : ResultArray)
		{
			InventoryComponent->AddItem(Item);
		}
	}
}
