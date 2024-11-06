// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClimCharacterMovementComponent.generated.h"


UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_CLimbing		UMETA(DisplayName = "Climbing"),
	CMOVE_MAX			UMETA(Hidden),
};

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UClimCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SweepAndStoreWallHits();

	

	// 벽과의 충돌 감지 켑슐콜리전
	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere)
	int CollsionCapsuleRadius = 50;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere)
	int CollsionCapsuleHeight = 72;

	TArray<FHitResult> CurrentWallHits;

	FCollisionQueryParams ClimbQueryParams;

private:
	// 등반하기위한 최소 각도
	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "75.0"))
	float MinHorizontalDegreesToStartClimbing = 25;

	bool CanStartClimbing();

private:
	bool EyeHeightTrace(const float TraceDistance) const;

	bool IsFacingSurface(const float SurfaceVerticalDot) const;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

public:
	void TryClimbing();

	void CancelClimbing();

	bool bWantsToClimbVeiw();

private:
	bool bWantsToClimb = false;

public:
	UFUNCTION(BlueprintPure)
	bool IsClimbing() const;

	UFUNCTION(BlueprintPure)
	FVector GetClimbSurfaceNormal() const;

private:
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	void PhysClimbing(float deltaTime, int32 Iterations);


private:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "80.0"))
	float ClimbingCollisionShrinkAmount = 30;

private:
	void ComputeSurfaceInfo();
	void ComputeClimbingVelocity(float deltaTime);
	bool ShouldStopClimbing();
	void StopClimbing(float deltaTime, int32 Iterations);
	void MoveAlongClimbingSurface(float deltaTime);
	void SnapToClimbingSurface(float deltaTime) const;

private:
	FVector CurrentClimbingNormal;
	FVector CurrentClimbingPosition;

private:
	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float MaxClimbingSpeed = 120.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "2000.0"))
	float MaxClimbingAcceleration = 380.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "3000.0"))
	float BrakingDecelerationClimbing = 550.f;

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;

private:
	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "12.0"))
	int ClimbingRotationSpeed = 6;

	FQuat GetClimbingRotation(float deltaTime) const;

private:
	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "60.0"))
	float ClimbingSnapSpeed = 4.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "80.0"))
	float DistanceFromSurface = 45.f;
	
private:
	bool ClimbDownToFloor() const;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "500.0"))
	float FloorCheckDistance = 100.f;

	bool CheckFloor(FHitResult& FloorHit) const;


};
