// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "C_WSWeapon.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSWeapon : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_WSWeapon();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void AttackDefault();

	virtual void AttackSecondary();
	
};
