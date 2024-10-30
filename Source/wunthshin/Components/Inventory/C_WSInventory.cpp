// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSInventory.h"

#include "wunthshin/Subsystem/WorldStatusSubsystem/WorldStatusSubsystem.h"
#include "wunthshin/Actors/Item/A_WSItem.h"

DEFINE_LOG_CATEGORY(LogInventory);

int32 UC_WSInventory::FindItemIndex(const USG_WSItemMetadata* InMetadata) const
{
	return Items.IndexOfByPredicate(
		[&InMetadata](const FInventoryPair& InPair)
		{
			return InPair.Metadata == InMetadata;
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


// Called when the game starts
void UC_WSInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UC_WSInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FInventoryPair* UC_WSInventory::FindItem(const USG_WSItemMetadata* InMetadata)
{
	return Items.FindByPredicate([&InMetadata](const FInventoryPair& InEntry)
		{
			return InEntry.Metadata == InMetadata;
		});
}

void UC_WSInventory::AddItem(AA_WSItem* InItem, int InCount)
{
	// 타입 캐스팅 안전
	if (InCount < 0) 
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

void UC_WSInventory::RemoveItem(AA_WSItem* InItem, int InCount)
{
	if (Items.IsEmpty())
	{
		return;
	}

	// 타입 캐스팅 안전
	if (InCount < 0) 
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

