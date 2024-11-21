// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"

#include "CommonPawn.generated.h"

class AwunthshinPlayerState;
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
class UCommonPawn : public UDataTableFetcher
{
	GENERATED_BODY()
};

/**
 * 
 */
class WUNTHSHIN_API ICommonPawn : public IDataTableFetcher
{
	GENERATED_BODY()

	// 맞으면 재생할 몽타주 setter
	virtual void SetHitMontages(const TArray<UAnimMontage*>& InMontages) = 0;
	
public:
	static const FName InventoryComponentName;

	COMPONENT_GETTER(UCapsuleComponent, CapsuleComponent);
	COMPONENT_GETTER(USkeletalMeshComponent, SkeletalMeshComponent);
	COMPONENT_GETTER(IInventoryComponent, InventoryComponent);
	COMPONENT_GETTER(UC_WSShield, ShieldComponent);
	COMPONENT_GETTER(UStatsComponent, StatsComponent);
	COMPONENT_GETTER(UChildActorComponent, RightHandComponent);
	COMPONENT_GETTER(UPawnMovementComponent, PawnMovementComponent);
	COMPONENT_GETTER(UC_WSSkill, SkillComponent);
	COMPONENT_GETTER(UDamageCounterPool, DamageCounters);

	// 빨리 달리는지 확인하는 함수
	virtual bool IsFastRunning() const = 0;
	// 걷고 있는지 확인하는 함수
	virtual bool IsWalking() const = 0;
	// 등반하고 있는지 확인하는 함수
	virtual bool CheckClimbState() const = 0;
	// 등반중 대쉬하는지 확인하는 함수
	virtual bool CheckClimbDashState() const = 0;
	// 대쉬 할때 Direction을 가져옴
	virtual FVector ClimbDashDirection() const = 0;
	
	// 스태미나가 다 떨어지면 호출되는 함수
	virtual void HandleStaminaDepleted() = 0;
	// 공통 PlayerState 참조 함수
	AwunthshinPlayerState* GetPlayerState() const;

	// Pawn 공통 업데이트 함수
	void UpdatePawnFromDataTable(const FCharacterTableRow* InData);

	// 맞은 모션 재생
	virtual void PlayHitMontage() = 0;
};
