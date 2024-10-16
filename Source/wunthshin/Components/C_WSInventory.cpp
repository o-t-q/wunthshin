// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSInventory.h"

DEFINE_LOG_CATEGORY(LogInventory);

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

void UC_WSInventory::AddItem(AActor* InItem, int Count)
{
	for (auto item : Items)
	{
		if (item->GetFName() == InItem->GetFName())
		{
			// todo : 소모품은 카운트 증가
			UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::AddItem"));
			return;
		}
	}

	// 장비 아이템 추가
	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::AddItem"));
	Items.Emplace(InItem);
}

void UC_WSInventory::RemoveItem(AActor* InItem, int Count)
{
	if (Items.IsEmpty())
	{
		return;
	}

	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::RemoveItem"));
	Items.Remove(InItem);
	Items.Shrink();
}

void UC_WSInventory::UseItem(AActor* InItem, int Count)
{
	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::UseItem"));
}

