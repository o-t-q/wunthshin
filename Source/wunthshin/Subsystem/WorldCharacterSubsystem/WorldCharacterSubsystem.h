// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldCharacterSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWorldCharacterSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> ItemsNearbyCharacter;

public:
	virtual void Tick(float InDeltaTime);

	TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UWorldCharacterSubsystem, STATGROUP_Tickables);
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
