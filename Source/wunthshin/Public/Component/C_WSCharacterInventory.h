// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/InventoryComponent.h"
#include "C_WSCharacterInventory.generated.h"

/**
 * 게임 인스턴스에 연동된 플레이어 귀속 인벤토리 컴포넌트
 * 플레이어 별로 생성되는 공유 인벤토리 프록시
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSCharacterInventory : public UActorComponent, public IInventoryComponent
{
	GENERATED_BODY()
	friend class UWSItemSubsystem;
	
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
	virtual void AddItem(const USG_WSItemMetadata* InMetadata, int InCount = 1) override;
	virtual void RemoveItem(const USG_WSItemMetadata* InItem, int InCount = 1) override;
	virtual void UseItem(uint32 Index, AActor* InTarget, int InCount = 1) override;

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_FetchInventory();
};
