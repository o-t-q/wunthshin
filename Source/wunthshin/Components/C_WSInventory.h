// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_WSInventory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WUNTHSHIN_API UC_WSInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_WSInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void AddItem(AActor* InItem, int Count = 1);	// 아이템 추가
	void RemoveItem(AActor* InItem, int Count = 1); // 아이템 빼기
	void UseItem(AActor* InItem, int Count = 1);	// 아이템 사용


protected:
	TArray<AActor*> Items;


};
