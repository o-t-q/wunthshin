// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

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
	class UPawnMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly)
	double WalkSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	double RunSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	double FastRunSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bShoudWalk = false;

	UPROPERTY(BlueprintReadOnly)
	bool bShoudRun = false;

	UPROPERTY(BlueprintReadOnly)
	bool bShoudFastRun = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouch = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling = false;

	// ���׹̳��ϰ� ���� �ʿ�
	UPROPERTY(BlueprintReadOnly)
	bool bStaminaEmpty = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCliming = false;

	// ������ ������� ����?
	UPROPERTY()
	double WallHight = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bwallHightLow = false;

	UPROPERTY(BlueprintReadOnly)
	bool bwallHightmid = false;
;
	UPROPERTY(BlueprintReadOnly)
	bool bwallHighthHigh = false;
};
