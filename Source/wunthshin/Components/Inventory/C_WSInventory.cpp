// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSInventory.h"

#include "wunthshin/Actors/Item/A_WSItem.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Item/ItemSubsystem.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"

class UWorldStatusSubsystem;
DEFINE_LOG_CATEGORY(LogInventory);

int32 UC_WSInventory::FindItemIndex(const USG_WSItemMetadata* InMetadata) const
{
	return Items.IndexOfByPredicate(
		[&InMetadata](const FInventoryPair& InPair)
		{
			return InPair.Metadata == InMetadata;
		});
}

FInventoryPair* UC_WSInventory::FindItem(const USG_WSItemMetadata* InMetadata)
{
	return Items.FindByPredicate([&InMetadata](const FInventoryPair& InEntry)
		{
			return InEntry.Metadata == InMetadata;
		});
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
	return Items;
}


// Called when the game starts
void UC_WSInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...

	
}

void UC_WSInventory::AddItem(AA_WSItem* InItem, int InCount)
{
	// 타입 캐스팅 안전
	if (InCount <= 0) 
	{
		return;
	}

	const USG_WSItemMetadata* ItemMetadata = InItem->GetItemMetadata();
	// Item metadata가 생성되지 않았음
	check(ItemMetadata);

	// 동일한 아이템이 이미 존재하는 경우
	if (FInventoryPair* Iterator = FindItem(ItemMetadata); Iterator)
	{
		UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::AddItem"));
		// todo: 오버플로우 방지
		Iterator->Count += InCount;
		return;
	}

	// 동일한 아이템이 없으므로 추가
	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::AddItem"));
	Items.Emplace(InItem->GetItemMetadata());
}

void UC_WSInventory::AddItem(USG_WSItemMetadata* InMetaData, int InCount)
{
	// 타입 캐스팅 안전
	if (InCount <= 0) 
	{
		return;
	}
	
	// Item metadata가 생성되지 않았음
	check(InMetaData);

	// 동일한 아이템이 이미 존재하는 경우
	if (FInventoryPair* Iterator = FindItem(InMetaData); Iterator)
	{
		UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::AddItem"));
		// todo: 오버플로우 방지
		Iterator->Count += InCount;
		return;
	}

	// 동일한 아이템이 없으므로 추가
	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::AddItem"));
	Items.Emplace(InMetaData);
}

void UC_WSInventory::AddItem(FInventoryPair InInvenPair)
{
	// 타입 캐스팅 안전
	if (InInvenPair.Count <= 0) 
	{
		return;
	}
	
	// Item metadata가 생성되지 않았음
	check(InInvenPair.Metadata);

	// 동일한 아이템이 이미 존재하는 경우
	if (FInventoryPair* Iterator = FindItem(InInvenPair.Metadata); Iterator)
	{
		UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::AddItem"));
		// todo: 오버플로우 방지
		Iterator->Count += InInvenPair.Count;
		return;
	}

	// 동일한 아이템이 없으므로 추가
	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::AddItem"));
	Items.Emplace(InInvenPair);
}

void UC_WSInventory::RemoveItem(AA_WSItem* InItem, int InCount)
{
	if (Items.IsEmpty())
	{
		return;
	}

	// 타입 캐스팅 안전
	if (InCount <= 0) 
	{
		return;
	}

	const USG_WSItemMetadata* ItemMetadata = InItem->GetItemMetadata();
	check(ItemMetadata);

	if (FInventoryPair* Iterator = FindItem(ItemMetadata); Iterator)
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

		UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::RemoveItem"));

		// 아이템을 버림
		// todo: 오버플로우, 언더플로우 방지
		Iterator->Count -= (uint32)InCount;

		// 아이템을 모두 버렸을 경우 Items에서 삭제
		if (Iterator->Count == 0UL)
		{
			Items.RemoveAll([&Iterator](const FInventoryPair& InPair)
				{
					return Iterator->Metadata == InPair.Metadata;
				});
		}
	}
}

void UC_WSInventory::UseItem(uint32 Index, AActor* InTarget, int Count)
{
	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::UseItem"));

	// 음수 갯수 예외처리
	if (Count <= 0) 
	{
		return;
	}

	// OOB
	if (Items.Num() < Count)
	{
		return;
	}

	// 사용 횟수가 소유하고 있는 아이템 수보다 많은 경우
	if (Items[Index].Count < static_cast<uint32>(Count))
	{
		return;
	}
	
	if (UWorldStatusSubsystem* WorldStatus = GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
	{
		for (int i = 0; i < Count; ++i) 
		{
			WorldStatus->PushItem(Items[Index].Metadata, GetOwner(), InTarget);
		}

		Items[Index].Count -= Count;

		if (Items[Index].Count <= 0)
		{
			Items.RemoveAt(Index);
		}
	}
}