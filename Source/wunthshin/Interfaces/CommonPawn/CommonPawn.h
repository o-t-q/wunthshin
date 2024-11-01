// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CommonPawn.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;
class UC_WSInventory;
class UC_WSShield;
class UStatsComponent;
class UChildActorComponent;

struct FCharacterTableRow;

#define COMPONENT_GETTER(ComponentType, ComponentName) \
	virtual class ComponentType* Get##ComponentName() const = 0;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCommonPawn : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WUNTHSHIN_API ICommonPawn
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FName GetAssetName() const = 0;

	COMPONENT_GETTER(UCapsuleComponent, CapsuleComponent);
	COMPONENT_GETTER(USkeletalMeshComponent, SkeletalMeshComponent);
	COMPONENT_GETTER(UC_WSInventory, InventoryComponent);
	COMPONENT_GETTER(UC_WSShield, ShieldComponent);
	COMPONENT_GETTER(UStatsComponent, StatsComponent);
	COMPONENT_GETTER(UChildActorComponent, RightHandComponent);
	COMPONENT_GETTER(UPawnMovementComponent, MovementComponent);

	// 빨리 달리는지 확인하는 함수
	virtual bool IsFastRunning() const = 0;
	// 걷고 있는지 확인하는 함수
	virtual bool IsWalking() const = 0;
	// 스태미나가 다 떨어지면 호출되는 함수
	virtual void HandleStaminaDepleted() = 0;

	void UpdatePawnFromDataTable(const FCharacterTableRow* InData);
};
