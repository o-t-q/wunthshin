// Fill out your copyright notice in the Description page of Project Settings.


#include "A_LootingBox.h"

#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"

#include "wunthshin/Subsystem/Utility.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Item/ItemSubsystem.h"


// Sets default values
AA_LootingBox::AA_LootingBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//PickUpComponent->OnPickUp
	
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

bool AA_LootingBox::Take(UC_WSPickUp* InTakenComponent)
{
	PickUpComponent = InTakenComponent;
	
	return true;
}

void AA_LootingBox::ApplyAsset(const FTableRowBase* InRowPointer)
{
	if(!InRowPointer) return;
	
	auto data = reinterpret_cast<const FLootingBoxTableRow*>(InRowPointer);
	Data = *const_cast<FLootingBoxTableRow*>(data);
	
	InitializeLootingBox();
}

void AA_LootingBox::InitializeLootingBox()
{
	auto RandomItems = Data.RandomItems;
	auto EssentialItems = Data.EssentialItems;
	auto Subsystem = GetGameInstance()->GetSubsystem<UItemSubsystem>();

	// 필수 아이템 초기화
	for(auto Item : EssentialItems)
	{
		// auto MetaData = Subsystem->GetMetadata<USG_WSItemMetadata>(this->GetWorld(),this, Item.ItemRowHandle.RowName);
		// uint64 Count = FMath::RandRange((double)Item.MinQuantity, (double)Item.MaxQuantity);
		// Items.Add(MetaData, Count);
	}

	// 랜덤 아이템 초기화
	for(auto Item : RandomItems)
	{
		
	}
}

void AA_LootingBox::Interaction()
{
	auto owner = Cast<AA_WSCharacter>(PickUpComponent->GetOwner());
	if(owner) return;

	auto inven = owner->GetComponentByClass<UC_WSInventory>();

	for(auto& Item : Items)
	{
		
	}

	Destroy();
}

