// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "wunthshin/Data/Items/InventoryPair/InventoryPair.h"
#include "InventoryComponent.generated.h"

class AA_WSItem;
class USG_WSItemMetadata;

// This class does not need to be modified.
/**
 * 여러개의 인벤토리 컴포넌트를 위한 인터페이스
 */
UINTERFACE()
class UInventoryComponent : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WUNTHSHIN_API IInventoryComponent
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual const TArray<FInventoryPair>& GetItems() const = 0;

	virtual int32 FindItemIndex(const USG_WSItemMetadata* InMetadata) const = 0;
	virtual FInventoryPair* FindItem(const USG_WSItemMetadata* InMetadata) = 0;
	
	virtual void AddItem(AA_WSItem* InItem, int InCount = 1) = 0;	// 아이템 추가
	virtual void AddItem(const USG_WSItemMetadata* InMetadata, int InCount = 1) = 0;	// 아이템 추가
	virtual void RemoveItem(const USG_WSItemMetadata* InItem, int InCount = 1) = 0; // 아이템 빼기
	virtual void UseItem(uint32 Index, AActor* InTarget, int InCount = 1) = 0;	// 아이템 사용
};
