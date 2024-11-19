// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "wunthshin/Data/Items/InventoryPair/InventoryPair.h"
#include "C_WSInventory.generated.h"

class UListView;
class UInventoryEntryData;
class USG_WSItemMetadata;
class AA_WSItem;
class UImage;

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

// ListView 동적 생성을 위해 UCLASS로 한번 랩핑함
UCLASS()
class WUNTHSHIN_API UInventoryEntryData : public UObject
{
	GENERATED_BODY()
	
public:
	UInventoryEntryData(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer) {};
	UInventoryEntryData(const FInventoryPair& InPair)
		: EntryData(InPair)	{}

	void Initialize(const FInventoryPair& InventoryPair, UListView* InListView, UObject* InOwner)
	{
		EntryData = InventoryPair;
		Root = InListView;
		Owner = InOwner;
	}
public:
	FInventoryPair EntryData;

	UPROPERTY()
	UListView* Root;

	UPROPERTY()
	UObject* Owner;
public:
	bool operator!=(const FInventoryPair& InOther) const
	{
		if(EntryData.Metadata == InOther.Metadata)
			return true;

		return false;
	}

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WUNTHSHIN_API UC_WSInventory : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FInventoryPair> Items;

public:
	// Sets default values for this component's properties
	UC_WSInventory();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual const TArray<FInventoryPair>& GetItems() const;

	virtual int32 FindItemIndex(const USG_WSItemMetadata* InMetadata) const;
	virtual FInventoryPair* FindItem(const USG_WSItemMetadata* InMetadata);
	
	virtual void AddItem(AA_WSItem* InItem, int InCount = 1);	// 아이템 추가
	virtual void RemoveItem(AA_WSItem* InItem, int InCount = 1); // 아이템 빼기
	virtual void UseItem(uint32 Index, AActor* InTarget, int InCount = 1);	// 아이템 사용
	
};
