// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/WSSharedInventory.h"

#include "Controller/wunthshinPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Network/Subsystem/WSServerSubsystem.h"
#include "Subsystem/WorldStatusSubsystem.h"

DEFINE_LOG_CATEGORY( LogSharedInventory );

const TArray<FInventoryPair>& AWSSharedInventory::GetItems() const
{
	return ItemsOwned; 
}

void AWSSharedInventory::AddItem( const USG_WSItemMetadata* InMetadata, const uint32 InCount )
{
	if (InCount <= 0)
	{
		return;
	}

	UE_LOG(LogSharedInventory, Log, TEXT("UC_WSInventory::AddItemInternal"));

	// 동일한 아이템이 이미 존재하는 경우
	if (FInventoryPair* Iterator = FindItem(InMetadata); Iterator)
	{
		// todo: 오버플로우 방지
		Iterator->Count += InCount;
		return;
	}

	// 동일한 아이템이 없으므로 추가
	ItemsOwned.Emplace( InMetadata, InCount );
}

void AWSSharedInventory::RemoveItem( const USG_WSItemMetadata* InItemMetadata, const uint32 InCount )
{
	if (InCount <= 0)
	{
		return;
	}
	
	if (ItemsOwned.IsEmpty())
	{
		return;
	}

	if (FInventoryPair* Iterator = FindItem(InItemMetadata); Iterator)
	{
		// 못 찾은 경우
		if (Iterator == nullptr) 
		{
			return;
		}

		// 버리고자 하는 값이 더 큼
		if (Iterator->Count < (uint32)InCount) 
		{
			return;
		}

		UE_LOG(LogSharedInventory, Log, TEXT("UC_WSInventory::RemoveItem"));

		// 아이템을 버림
		// todo: 오버플로우, 언더플로우 방지
		Iterator->Count -= (uint32)InCount;

		// 아이템을 모두 버렸을 경우 Items에서 삭제
		if (Iterator->Count == 0UL)
		{
			ItemsOwned.RemoveAll([&Iterator](const FInventoryPair& InPair)
				{
					return Iterator->Metadata == InPair.Metadata;
				});
		}
	}
}

void AWSSharedInventory::UseItem( uint32 InIndex, AActor* InUser, AActor* InTargetActor, uint32 InCount )
{
	if (InCount <= 0)
	{
		return;
	}
	
	// OOB
	if (static_cast<uint32>(ItemsOwned.Num()) < InCount)
	{
		return;
	}

	// 사용 횟수가 소유하고 있는 아이템 수보다 많은 경우
	if (ItemsOwned[InIndex].Count < static_cast<uint32>(InCount))
	{
		return;
	}
	
	if (UWorldStatusSubsystem* WorldStatus = InTargetActor->GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
	{
		for (uint32 i = 0; i < InCount; ++i) 
		{
			WorldStatus->PushItem(ItemsOwned[InIndex].Metadata, InUser, InTargetActor);
		}

		ItemsOwned[InIndex].Count -= InCount;

		if (ItemsOwned[InIndex].Count <= 0)
		{
			ItemsOwned.RemoveAt(InIndex);
		}
	}
}

void AWSSharedInventory::Clear( const int32 Reserve )
{
	ItemsOwned.Empty();

	if (Reserve != 0) 
	{
		ItemsOwned.Reserve(Reserve);
	}
}

int32 AWSSharedInventory::FindItemIndex( const USG_WSItemMetadata* InMetadata ) const
{
	return ItemsOwned.IndexOfByPredicate(
		[&InMetadata](const FInventoryPair& InPair)
		{
			return InPair.Metadata == InMetadata;
		});
}

FInventoryPair* AWSSharedInventory::FindItem( const USG_WSItemMetadata* InMetadata )
{
	return ItemsOwned.FindByPredicate([&InMetadata](const FInventoryPair& InEntry)
		{
			return InEntry.Metadata == InMetadata;
		});
}

void AWSSharedInventory::Server_FetchInventory_Implementation()
{
	if (UWSServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UWSServerSubsystem>())
	{
		const bool Result = ServerSubsystem->Client_TryGetItems( Cast<AwunthshinPlayerController>(GetWorld()->GetFirstPlayerController()), 0 );
		check( Result );
	}
}

bool AWSSharedInventory::Server_FetchInventory_Validate()
{
	// todo: rate limit
	return true;
}

void AWSSharedInventory::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION( AWSSharedInventory, ItemsOwned, COND_OwnerOnly )
}
