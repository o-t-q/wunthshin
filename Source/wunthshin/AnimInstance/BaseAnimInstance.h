// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

class AA_WSCharacter;
class UPawnMovementComponent;

/**
 * 
 */
UCLASS()
class UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY()
	UPawnMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly)
	double HorizontalSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	double VerticalSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bShoudWalk = false;

	UPROPERTY(BlueprintReadOnly)
	bool bShoudFastRun = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouch = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsGlide = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCliming = false;

	UPROPERTY()
	double WallHight = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bwallHightLow = false;

	UPROPERTY(BlueprintReadOnly)
	bool bwallHightmid = false;
;
	UPROPERTY(BlueprintReadOnly)
	bool bwallHighthHigh = false;

	UPROPERTY()
	AA_WSCharacter* CharaterComponentRef;

public:
	//void SetRootMotionMode();

	
};
