// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "C_WSCharacterInventory.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSCharacterInventory : public UC_WSInventory
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_WSCharacterInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual const TArray<FInventoryPair>& GetItems() const override;

	virtual int32 FindItemIndex(const USG_WSItemMetadata* InMetadata) const override;
	virtual FInventoryPair* FindItem(const USG_WSItemMetadata* InMetadata) override;
	
	virtual void AddItem(AA_WSItem* InItem, int InCount = 1) override;
	virtual void RemoveItem(AA_WSItem* InItem, int InCount = 1) override;
	virtual void UseItem(uint32 Index, AActor* InTarget, int InCount = 1) override;
};
