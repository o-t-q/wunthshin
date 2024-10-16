// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_WSInventory.generated.h"

class AA_WSItem;
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WUNTHSHIN_API UC_WSInventory : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess))
	TArray<AA_WSItem*> Items;

public:	
	// Sets default values for this component's properties
	UC_WSInventory();

	const TArray<AA_WSItem*>& GetItems() const { return Items; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void AddItem(AA_WSItem* InItem, int Count = 1);	// 아이템 추가
	void RemoveItem(AA_WSItem* InItem, int Count = 1); // 아이템 빼기
	void UseItem(AA_WSItem* InItem, int Count = 1);	// 아이템 사용

};
