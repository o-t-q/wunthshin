// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "C_WSShield.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShieldComponent, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSShield : public UStaticMeshComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Duration", meta=(AllowPrivateAccess = "true"))
	float ShieldTimer;

	FTimerHandle ShieldTimerHandle;
	
public:
	// Sets default values for this component's properties
	UC_WSShield();

	void SetShieldTime(const float InTime) { ShieldTimer = InTime; }

	virtual void SetActive(bool bNewActive, bool bReset = false) override;

	virtual void InitializeComponent() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
