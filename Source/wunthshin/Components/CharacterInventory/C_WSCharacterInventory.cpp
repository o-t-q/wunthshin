// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSCharacterInventory.h"

#include "wunthshin/Actors/Item/A_WSItem.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Item/ItemSubsystem.h"


// Sets default values for this component's properties
UC_WSCharacterInventory::UC_WSCharacterInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UC_WSCharacterInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

const TArray<FInventoryPair>& UC_WSCharacterInventory::GetItems() const
{
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		return ItemSubsystem->GetSharedInventory().GetItems();
	}

	return Super::GetItems();
}

int32 UC_WSCharacterInventory::FindItemIndex(const USG_WSItemMetadata* InMetadata) const
{
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		return ItemSubsystem->GetSharedInventory().FindItemIndex(InMetadata);
	}

	return -1;
}

FInventoryPair* UC_WSCharacterInventory::FindItem(const USG_WSItemMetadata* InMetadata)
{
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		return ItemSubsystem->GetSharedInventory().FindItem(InMetadata);
	}

	return nullptr;
}

void UC_WSCharacterInventory::AddItem(AA_WSItem* InItem, int InCount)
{
	// 타입 캐스팅 안전
	if (InCount < 0) 
	{
		return;
	}

	const USG_WSItemMetadata* ItemMetadata = InItem->GetItemMetadata();
	// Item metadata가 생성되지 않았음
	check(ItemMetadata);

	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		ItemSubsystem->GetSharedInventory().AddItem(ItemMetadata, InCount);
	}
}

void UC_WSCharacterInventory::RemoveItem(AA_WSItem* InItem, int InCount)
{
	// 타입 캐스팅 안전
	if (InCount < 0) 
	{
		return;
	}

	const USG_WSItemMetadata* ItemMetadata = InItem->GetItemMetadata();
	check(ItemMetadata);

	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		ItemSubsystem->GetSharedInventory().RemoveItem(ItemMetadata, InCount);
	}
}

void UC_WSCharacterInventory::UseItem(uint32 Index, AActor* InTarget, int InCount)
{
	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::UseItem"));

	// 음수 갯수 예외처리
	if (InCount < 0) 
	{
		return;
	}
	
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		ItemSubsystem->GetSharedInventory().UseItem(Index, GetOwner(), InTarget, InCount);
	}
}


