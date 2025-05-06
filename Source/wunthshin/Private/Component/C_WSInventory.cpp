// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/C_WSInventory.h"

#include "Actor/Item/A_WSItem.h"
#include "Data/Item/WSSharedInventory.h"
#include "Net/UnrealNetwork.h"
#include "Subsystem/WSItemSubsystem.h"
#include "Subsystem/WorldStatusSubsystem.h"

class UWorldStatusSubsystem;
DEFINE_LOG_CATEGORY(LogInventory);

int32 UC_WSInventory::FindItemIndex(const USG_WSItemMetadata* InMetadata) const
{
	return Items->FindItemIndex(InMetadata);
}

FInventoryPair* UC_WSInventory::FindItem(const USG_WSItemMetadata* InMetadata)
{
	return Items->FindItem(InMetadata);
}


// Sets default values for this component's properties
UC_WSInventory::UC_WSInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...


}

const TArray<FInventoryPair>& UC_WSInventory::GetItems() const
{
	return Items->GetItems();
}


// Called when the game starts
void UC_WSInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...

	
}

void UC_WSInventory::GetLifetimeReplicatedProps( TArray<class FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );
	// todo: client knows which items they is holding
	DOREPLIFETIME( UC_WSInventory, Items )
}

void UC_WSInventory::AddItem(AA_WSItem* InItem, int InCount)
{
	if (const USG_WSItemMetadata* ItemMetadata = InItem->GetItemMetadata())
	{
		AddItem(ItemMetadata, InCount);
	}
}

void UC_WSInventory::AddItem(const USG_WSItemMetadata* InMetadata, int InCount)
{
	Items->AddItem(InMetadata, InCount);
}

void UC_WSInventory::RemoveItem(const USG_WSItemMetadata* InItem, int InCount)
{
	Items->RemoveItem(InItem, InCount);
}

void UC_WSInventory::UseItem(uint32 Index, AActor* InTarget, int Count)
{
	Items->UseItem(Index, GetOwner(),InTarget, Count);
}
