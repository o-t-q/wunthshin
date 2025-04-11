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

public:

	UClimCharacterMovementComponent();

	UFUNCTION(BlueprintCallable)
	void TryClimbing();

	UFUNCTION(BlueprintCallable)
	void CancelClimbing();

	UFUNCTION(BlueprintPure)
	bool bWantsToClimbVeiw();


	UFUNCTION(BlueprintPure)
	bool IsClimbing() const;

	UFUNCTION(BlueprintPure)
	bool CanStartClimbing();

	UFUNCTION(BlueprintPure)
	FVector GetClimbSurfaceNormal() const;

	UFUNCTION(BlueprintCallable)
	void TryClimbDashing();

	UFUNCTION(BlueprintPure)
	bool IsClimbDashing() const{ return IsClimbing() && bIsClimbDashing; }

	UFUNCTION(BlueprintPure)
	FVector GetClimbDashDirection() const{ return ClimbDashDirection; }


private:

	UPROPERTY(Category = "Character Movement: Climbing", EditDefaultsOnly)
	UCurveFloat* ClimbDashCurve;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere)
	int CollsionCapsuleRadius = 50;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere)
	int CollsionCapsuleHeight = 72;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "75.0"))
	float MinHorizontalDegreesToStartClimbing = 25;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "80.0"))
	float ClimbingCollisionShrinkAmount = 30;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float MaxClimbingSpeed = 120.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "2000.0"))
	float MaxClimbingAcceleration = 380.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "3000.0"))
	float BrakingDecelerationClimbing = 550.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "60.0"))
	float ClimbingSnapSpeed = 4.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "80.0"))
	float DistanceFromSurface = 45.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "12.0"))
	int ClimbingRotationSpeed = 6;

	UPROPERTY(Category = "Character Movement: Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "500.0"))
	float FloorCheckDistance = 80.f;

	UPROPERTY(Category = "Character Movement: Climbing", EditDefaultsOnly)
	UAnimMontage* LedgeClimbMontage;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	TArray<FHitResult> CurrentWallHits;

	FCollisionQueryParams ClimbQueryParams;

	FVector CurrentClimbingNormal;

	FVector CurrentClimbingPosition;

	FVector ClimbDashDirection;

	float CurrentClimbDashTime;

	bool bIsClimbDashing = false;

	bool bWantsToClimb = false;


private:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual float GetMaxSpeed() const override;

	virtual float GetMaxAcceleration() const override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	void SweepAndStoreWallHits();

	void PhysClimbing(float deltaTime, int32 Iterations);

	void ComputeSurfaceInfo();

	void ComputeClimbingVelocity(float deltaTime);

	void StopClimbing(float deltaTime, int32 Iterations);

	void MoveAlongClimbingSurface(float deltaTime);

	void SnapToClimbingSurface(float deltaTime) const;

	void UpdateClimbDashState(float deltaTime);

	void StopClimbDashing();

	void AlignClimbDashDirection();

	void StoreClimbDashDirection();

	FQuat GetClimbingRotation(float deltaTime) const;

	bool EyeHeightTrace(const float TraceDistance) const;

	bool IsFacingSurface(const float SurfaceVerticalDot) const;

	bool ShouldStopClimbing();

	bool ClimbDownToFloor() const;

	bool CheckFloor(FHitResult& FloorHit) const;

	bool TryClimbUpLedge() const;

	bool HasReachedEdge() const;

	bool IsLocationWalkable(const FVector& CheckLocation) const;

	bool CanMoveToLedgeClimbLocation() const;



	


	


};
