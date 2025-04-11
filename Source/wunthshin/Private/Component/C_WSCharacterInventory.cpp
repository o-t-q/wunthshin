// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/C_WSCharacterInventory.h"

#include "Actor/Item/A_WSItem.h"
#include "Subsystem/CharacterSubsystem.h"
#include "Subsystem/ItemSubsystem.h"

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
	static const TArray<FInventoryPair> EmptyFallbackReturn = {};
	
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		return ItemSubsystem->GetSharedInventory().GetItems();
	}

	// 게임 인스턴스에 접근 실패
	check(false);
	return EmptyFallbackReturn;
}

int32 UC_WSCharacterInventory::FindItemIndex(const USG_WSItemMetadata* InMetadata) const
{
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		return ItemSubsystem->GetSharedInventory().FindItemIndex(InMetadata);
	}

	return INDEX_NONE;
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
	if (const USG_WSItemMetadata* ItemMetadata = InItem->GetItemMetadata())
	{
		AddItem(ItemMetadata, InCount);
	}
}

void UC_WSCharacterInventory::AddItem(const USG_WSItemMetadata* InMetadata, int InCount)
{
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		if (InMetadata)
		{
			ItemSubsystem->GetSharedInventory().AddItem(InMetadata, InCount);
			OnCharacterInventoryUpdated.Broadcast();
		}
	}
}

void UC_WSCharacterInventory::RemoveItem(const USG_WSItemMetadata* InItem, int InCount)
{
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		ItemSubsystem->GetSharedInventory().AddItem(InItem, InCount);
		OnCharacterInventoryUpdated.Broadcast();
	}
}

void UC_WSCharacterInventory::UseItem(uint32 Index, AActor* InTarget, int InCount)
{
	//UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::UseItem"));

	
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		ItemSubsystem->GetSharedInventory().UseItem(Index, GetOwner(), InTarget, InCount);
		OnCharacterInventoryUpdated.Broadcast();
	}
}


