// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/C_WSCharacterInventory.h"
#include "Network/Subsystem/WSServerSubsystem.h"
#include "Data/Item/SG_WSItemMetadata.h"

#include "Actor/Item/A_WSItem.h"
#include "Controller/AwunthshinSpawnPlayerController.h"
#include "Data/Item/WSSharedInventory.h"

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
	if ( GetNetMode() == NM_Client )
	{
		// todo: listen server case
		Server_FetchInventory();
	}
}

const TArray<FInventoryPair>& UC_WSCharacterInventory::GetItems() const
{
	static const TArray<FInventoryPair> EmptyFallbackReturn = {};
	
	if (AwunthshinSpawnPlayerController* PlayerController = Cast<AwunthshinSpawnPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		return PlayerController->GetSharedInventory()->GetItems();
	}

	// 게임 인스턴스에 접근 실패
	check(false);
	return EmptyFallbackReturn;
}

int32 UC_WSCharacterInventory::FindItemIndex(const USG_WSItemMetadata* InMetadata) const
{
	if (AwunthshinSpawnPlayerController* PlayerController = Cast<AwunthshinSpawnPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		return PlayerController->GetSharedInventory()->FindItemIndex(InMetadata);
	}

	return INDEX_NONE;
}

FInventoryPair* UC_WSCharacterInventory::FindItem(const USG_WSItemMetadata* InMetadata)
{
	if ( const AwunthshinSpawnPlayerController* PlayerController = Cast<AwunthshinSpawnPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		return PlayerController->GetSharedInventory()->FindItem(InMetadata);
	}

	return nullptr;
}

void UC_WSCharacterInventory::AddItem(AA_WSItem* InItem, int InCount)
{
	if ( const USG_WSItemMetadata* ItemMetadata = InItem->GetItemMetadata() )
	{
		AddItem(ItemMetadata, InCount);
	}
}

void UC_WSCharacterInventory::AddItem(const USG_WSItemMetadata* InMetadata, int InCount)
{
	if (UWSServerSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>())
	{
		Subsystem->TryAddItem(InMetadata->GetItemType(), InMetadata->GetID(), InCount);
	}
}

void UC_WSCharacterInventory::RemoveItem(const USG_WSItemMetadata* InItem, int InCount)
{
	
}

void UC_WSCharacterInventory::UseItem(uint32 Index, AActor* InTarget, int InCount)
{
}

void UC_WSCharacterInventory::Server_FetchInventory_Implementation()
{
	const UWSServerSubsystem* ServerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
	check( ServerSubsystem );
	
	if ( const AwunthshinSpawnPlayerController* PlayerController = Cast<AwunthshinSpawnPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		const bool Result = ServerSubsystem->Server_GetItems( PlayerController, 0 );
		check( Result );
	}
}

bool UC_WSCharacterInventory::Server_FetchInventory_Validate()
{
	// todo: Rate limit
	return true;
}

