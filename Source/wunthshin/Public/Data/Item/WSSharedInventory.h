// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryPair.h"
#include "WSSharedInventory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSharedInventory, Log, All);

class USG_WSItemMetadata;

/**
 * 리플리케이션을 지원하며 인벤토리 기능을 수행하는 공용 클래스 (NPC, Character...)
 */
UCLASS(BlueprintType)
class WUNTHSHIN_API AWSSharedInventory : public AInfo
{
	GENERATED_BODY()

public:
	const TArray<FInventoryPair>& GetItems() const;
	void AddItem(const USG_WSItemMetadata* InMetadata, const uint32 InCount);
	void RemoveItem(const USG_WSItemMetadata* InItemMetadata, const uint32 InCount);
	void UseItem(uint32 InIndex, AActor* InUser, AActor* InTargetActor, uint32 InCount);
	void Clear(const int32 Reserve = 0);

	int32 FindItemIndex(const USG_WSItemMetadata* InMetadata) const;
	FInventoryPair* FindItem(const USG_WSItemMetadata* InMetadata);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_FetchInventory();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"), Replicated)
	TArray<FInventoryPair> ItemsOwned;
};
