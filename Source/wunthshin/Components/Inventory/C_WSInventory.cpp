// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSInventory.h"

#include "wunthshin/Actors/Item/A_WSItem.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Item/ItemSubsystem.h"

DEFINE_LOG_CATEGORY(LogInventory);

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
	UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>();
	check(ItemSubsystem);
	return ItemSubsystem->GetSharedInventory().GetItems();
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

	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		ItemSubsystem->GetSharedInventory().AddItem(ItemMetadata, InCount);
	}
}

void UC_WSInventory::RemoveItem(AA_WSItem* InItem, int InCount)
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

void UC_WSInventory::UseItem(uint32 Index, AActor* InTarget, int Count)
{
	UE_LOG(LogInventory, Log, TEXT("UC_WSInventory::UseItem"));

	// 음수 갯수 예외처리
	if (Count < 0) 
	{
		return;
	}
	
	if (UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>())
	{
		ItemSubsystem->GetSharedInventory().UseItem(Index, GetOwner(), InTarget, Count);
	}
}