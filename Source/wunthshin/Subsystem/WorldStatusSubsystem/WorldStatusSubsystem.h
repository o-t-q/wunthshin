// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldStatusSubsystem.generated.h"

class USG_WSItemMetadata;
class AA_WSItem;

struct FItemTicket 
{
	friend class UWorldStatusSubsystem;

	const USG_WSItemMetadata* Item;
	AActor* Instigator;
	AActor* Target;
	float Rate = -1.f;
	
	bool IsValid() const { return !bDisposed; }

	FTimerHandle& GetTimerHandle() { return TimerHandle; }

private:
	static void ExecuteAndAdjustLifetime(FItemTicket& InTicket);

	bool bDisposed = false;

	FTimerHandle TimerHandle;

	uint32 ExecuteCount = 0;
	uint32 MaxExecuteCount = 0;
};

/**
 * 현재 월드의 아이템 적용 및 캐릭터 관련 정보를 추적하는 서브시스템
 */
UCLASS()
class WUNTHSHIN_API UWorldStatusSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> ItemsNearbyCharacter;

	TArray<FItemTicket> ItemQueue;

public:
	virtual void Tick(float InDeltaTime);

	void PushItem(const USG_WSItemMetadata* InItem, AActor* InInstigator, AActor* InTarget);

	void PushItem(FItemTicket InItemTicket);

	TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UWorldStatusSubsystem, STATGROUP_Tickables);
	}

	UFUNCTION(BlueprintCallable)
	AActor* GetNearestItem() const 
	{
		if (ItemsNearbyCharacter.IsEmpty()) 
		{
			return nullptr;
		}

		return ItemsNearbyCharacter[0];
	}

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetNearestItems() const { return ItemsNearbyCharacter; }

	UFUNCTION(BlueprintCallable)
	bool IsItemNearest(const AActor* InActor) 
	{
		if (ItemsNearbyCharacter.IsEmpty()) 
		{
			return false;
		}

		return ItemsNearbyCharacter[0] == InActor;
	}
	
};
