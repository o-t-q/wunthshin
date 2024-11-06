// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshin/Components/ClimCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"


void UClimCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	ClimbQueryParams.AddIgnoredActor(GetOwner());
}

void UClimCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SweepAndStoreWallHits();
}

void UClimCharacterMovementComponent::SweepAndStoreWallHits()
{
	//캡슐 생성
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollsionCapsuleRadius, CollsionCapsuleHeight);
	//캡슐 위치 조정
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();


	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(
		Hits, // 감지된 충돌 정보를 저장할 배열
		Start, // 시작위치
		End, // 끝위치
		FQuat::Identity, // 회전 값 (형상의 회전) (여기서는 회전 없음)
		ECC_WorldStatic, // 충돌 채널 // 나중에 무기하고 적은 빼야함 벽만포함하게 바뀌어야함
		CollisionShape, // 충돌 형상
		ClimbQueryParams); 

	// 디버그에서 확인
	{
		DrawDebugCapsule(
			GetWorld(),
			Start,
			CollsionCapsuleHeight , // 캡슐의 반높이
			CollsionCapsuleRadius, // 캡슐의 반지름
			FQuat::Identity, // 회전
			FColor::Green, // 색상
			false, // 지속 시간 사용 여부 (false일 경우 한 프레임만 표시)
			1.0f // 표시 지속 시간 (초 단위)

		);
		

		for (const FHitResult& Hit : Hits)
		{
			// 충돌된 위치에 점을 표시
			DrawDebugPoint(
				GetWorld(),
				Hit.ImpactPoint, // 충돌 위치
				10.0f, // 점의 크기
				FColor::Red, 
				false, 
				1.0f 
			);

			// 충돌된 위치와 표면의 노멀을 표시 (선으로)
			DrawDebugLine(
				GetWorld(),
				Hit.ImpactPoint, 
				Hit.ImpactPoint + Hit.ImpactNormal * 50.0f, // 노멀 방향으로 선 그리기
				FColor::Yellow, 
				false,
				1.0f
			);
		}

	}


	
	if (HitWall)
	{
		CurrentWallHits = Hits; // Hits 배열을 CurrentWallHits에 할당
	}
	else
	{
		CurrentWallHits.Reset(); // 배열 초기화
	}
	
}

bool UClimCharacterMovementComponent::bWantsToClimbVeiw()
{
	return bWantsToClimb;
}

bool UClimCharacterMovementComponent::CanStartClimbing()
{
	for (FHitResult& Hit : CurrentWallHits)
	{
		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D(); // 벽과 충돌된 노말백터의 xy만 가져옴

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal); // 캐릭터의 기울기
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);  //물체 자체의 경사도

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot)); //케릭터 기울기의 각도

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);
		//          케릭터의 기울어진 각도 많이 기울어지면 false             충돌된 벽이 천장인가?   캐릭터의 눈정도의 위치에서의 라인트레이서가 벽의 높이를 체크
		if (HorizontalDegrees <= MinHorizontalDegreesToStartClimbing && !bIsCeiling && IsFacingSurface(VerticalDot))
		{
			return true;
		}
	}
	return false;
}

bool UClimCharacterMovementComponent::EyeHeightTrace(const float TraceDistance) const
{
	FHitResult UpperEdgeHit;

	const FVector Start = UpdatedComponent->GetComponentLocation() +
		(UpdatedComponent->GetUpVector() * GetCharacterOwner()->BaseEyeHeight);
	const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);
	// 라인 트레이스를 시각화
	DrawDebugLine(
		GetWorld(),
		Start,         // 시작 위치
		End,           // 끝 위치
		FColor::Green, // 선의 색상
		false,         // 지속 시간 사용 여부
		5.0f           // 지속 시간 (초 단위)
	);

	

	return GetWorld()->LineTraceSingleByChannel(UpperEdgeHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UClimCharacterMovementComponent::IsFacingSurface(const float SurfaceVerticalDot) const
{
	constexpr float BaseLength = 80;
	const float SteepnessMultiplier = 1 + (1 - SurfaceVerticalDot) * 5;

	// 경사로에 따른 라인트레이서의 길이를 조절
	return EyeHeightTrace(BaseLength * SteepnessMultiplier);
}

void UClimCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	if (CanStartClimbing())
	{
		if (bWantsToClimb)
		{
			SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_CLimbing);
		}
	}
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UClimCharacterMovementComponent::TryClimbing()
{
	if (CanStartClimbing())
	{
		bWantsToClimb = true;
	}
}

void UClimCharacterMovementComponent::CancelClimbing()
{
	bWantsToClimb = false;
}

bool UClimCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_CLimbing;
}

FVector UClimCharacterMovementComponent::GetClimbSurfaceNormal() const
{
	/*if (CurrentWallHits.Num() >0)
	{
		return  CurrentWallHits[0].Normal;
	}
	return  FVector::Zero();*/

	return CurrentClimbingNormal;
	
}

void UClimCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (CustomMovementMode == ECustomMovementMode::CMOVE_CLimbing)
	{
		PhysClimbing(deltaTime, Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UClimCharacterMovementComponent::PhysClimbing(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	ComputeSurfaceInfo();

	if (ShouldStopClimbing())
	{
		StopClimbing(deltaTime, Iterations);
		return;
	}

	ComputeClimbingVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	MoveAlongClimbingSurface(deltaTime);

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	if (ShouldStopClimbing() || ClimbDownToFloor()) // <--
	{
		StopClimbing(deltaTime, Iterations);
		return;
	}

	SnapToClimbingSurface(deltaTime);
}

void UClimCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - ClimbingCollisionShrinkAmount);
	}

	const bool bWasClimbing = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_CLimbing;
	if (bWasClimbing)
	{
		bOrientRotationToMovement = true;

		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + ClimbingCollisionShrinkAmount);

		// After exiting climbing mode, reset velocity and acceleration
		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UClimCharacterMovementComponent::ComputeSurfaceInfo()
{
	// Temporary code!
	//if (CurrentWallHits.Num() > 0)
	//{
	//	CurrentClimbingNormal = CurrentWallHits[0].Normal;
	//	CurrentClimbingPosition = CurrentWallHits[0].ImpactPoint;
	//}
	CurrentClimbingNormal = FVector::ZeroVector;
	CurrentClimbingPosition = FVector::ZeroVector;


	if (CurrentWallHits.IsEmpty())
	{
		return;
	}
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(6);

	for (const FHitResult& WallHit : CurrentWallHits)
	{
		const FVector End = Start + (WallHit.ImpactPoint - Start).GetSafeNormal() * 120;

		FHitResult AssistHit;
		GetWorld()->SweepSingleByChannel(AssistHit, Start, End, FQuat::Identity,
			ECC_WorldStatic, CollisionSphere, ClimbQueryParams);

		CurrentClimbingPosition += AssistHit.ImpactPoint;
		CurrentClimbingNormal += AssistHit.Normal;
	}

	CurrentClimbingPosition /= CurrentWallHits.Num();
	CurrentClimbingNormal = CurrentClimbingNormal.GetSafeNormal();
}

void UClimCharacterMovementComponent::ComputeClimbingVelocity(float deltaTime)
{
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		constexpr float Friction = 0.0f;
		constexpr bool bFluid = false;
		CalcVelocity(deltaTime, Friction, bFluid, BrakingDecelerationClimbing);
	}

	ApplyRootMotionToVelocity(deltaTime);
}

bool UClimCharacterMovementComponent::ShouldStopClimbing()
{
	const bool bIsOnCeiling = FVector::Parallel(CurrentClimbingNormal, FVector::UpVector);

	return !bWantsToClimb || CurrentClimbingNormal.IsZero() || bIsOnCeiling;
}

void UClimCharacterMovementComponent::StopClimbing(float deltaTime, int32 Iterations)
{
	bWantsToClimb = false;
	SetMovementMode(EMovementMode::MOVE_Falling);
	StartNewPhysics(deltaTime, Iterations);
}

void UClimCharacterMovementComponent::MoveAlongClimbingSurface(float deltaTime)
{
	const FVector Adjusted = Velocity * deltaTime;

	FHitResult Hit(1.f);

	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
}
// 벽에서 떨어지지 않게
void UClimCharacterMovementComponent::SnapToClimbingSurface(float deltaTime) const
{
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector Location = UpdatedComponent->GetComponentLocation();
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();

	const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
	const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length() - DistanceFromSurface);

	constexpr bool bSweep = true;
	UpdatedComponent->MoveComponent(Offset * ClimbingSnapSpeed * deltaTime, Rotation, bSweep);
}

float UClimCharacterMovementComponent::GetMaxSpeed() const
{
	return IsClimbing() ? MaxClimbingSpeed : Super::GetMaxSpeed();
}

float UClimCharacterMovementComponent::GetMaxAcceleration() const
{
	return IsClimbing() ? MaxClimbingAcceleration : Super::GetMaxAcceleration();
}

FQuat UClimCharacterMovementComponent::GetClimbingRotation(float deltaTime) const
{
	const FQuat Current = UpdatedComponent->GetComponentQuat();
	const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();

	return FMath::QInterpTo(Current, Target, deltaTime, ClimbingRotationSpeed);
}

bool UClimCharacterMovementComponent::ClimbDownToFloor() const
{
	FHitResult FloorHit;
	if (!CheckFloor(FloorHit))
	{
		return false;
	}

	const bool bOnWalkableFloor = FloorHit.Normal.Z > GetWalkableFloorZ();

	const float DownSpeed = FVector::DotProduct(Velocity, -FloorHit.Normal);
	const bool bIsMovingTowardsFloor = DownSpeed >= MaxClimbingSpeed / 3 && bOnWalkableFloor;

	const bool bIsClimbingFloor = CurrentClimbingNormal.Z > GetWalkableFloorZ();

	return bIsMovingTowardsFloor || (bIsClimbingFloor && bOnWalkableFloor);
}

bool UClimCharacterMovementComponent::CheckFloor(FHitResult& FloorHit) const
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + FVector::DownVector * FloorCheckDistance;

	return GetWorld()->LineTraceSingleByChannel(FloorHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}


