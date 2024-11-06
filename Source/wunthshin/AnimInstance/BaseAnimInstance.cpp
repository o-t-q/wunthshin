// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "wunthshin/Interfaces/CommonPawn/CommonPawn.h"
#include "Kismet/KismetMathLibrary.h"




void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	APawn* Pawn = TryGetPawnOwner();
	if (GIsEditor && FApp::IsGame() && !Pawn)
	{
		checkf(false, TEXT("UBaisicCharacterAnimInstance를 사용하려면 소유권자가 Pawn이여야 합니다."));
		return;
	}
	else if (!Pawn) { return; }
	MovementComponent = Pawn->GetMovementComponent();
	check(MovementComponent);
	
	//CharateromponentRef = GetMove
	
	CharaterComponentRef = Pawn;
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	

	if (!MovementComponent) { return; }

	const FVector& Velocity = CharaterComponentRef->GetPawnMovementComponent()->Velocity;
	const double MaxSpeed = CharaterComponentRef->GetPawnMovementComponent()->GetMaxSpeed();

	if (FVector2D XY = {Velocity.X, Velocity.Y};
		XY.Length() == 0)
	{
		HorizontalSpeed = 0;
	}
	else 
	{
		// 닷 프로덕트 -> 앞 우의 길이 -> 앱솔루트 (+ 음수영역인 뒤, 좌를 접음)
		HorizontalSpeed = FMath::Abs(Velocity.Dot(FVector::ForwardVector + FVector::RightVector));
	}

	if (float Z = Velocity.Z;
		Z == 0) 
	{
		VerticalSpeed = 0;
	}
	else 
	{
		VerticalSpeed = FMath::Abs(Velocity.Dot(FVector::UpVector));
	}

	HorizontalSpeed = FMath::Lerp(0.f, 1.f, HorizontalSpeed / MaxSpeed);
	VerticalSpeed = FMath::Lerp(0.f, 1.f, VerticalSpeed / MaxSpeed);
	
	const FTransform ActorTransfor = Cast<AActor>(CharaterComponentRef.GetInterface())->GetActorTransform();
	const FVector LocalVelocity = ActorTransfor.InverseTransformVector(Velocity);
	
	VelocityBlendspaceY = LocalVelocity.Y;
	VelocityBlendspaceZ = Velocity.Z;
	bShoudFastRun = CharaterComponentRef->IsFastRunning();
	bShoudWalk = CharaterComponentRef->IsWalking();
	bIsCliming = CharaterComponentRef->CheckClimbState();
	bIsCrouch = MovementComponent->IsCrouching();
	bIsFalling = MovementComponent->IsFalling();

}



//void UBaseAnimInstance::SetRootMotionMode()
//{
//	ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
//	if (Character)
//	{
//		UCharacterMovementComponent* MovementConponent = Character->GetCharacterMovement();
//		{
//			if (MovementComponent)
//			{
//				MovementComponent->bRootMotionMode
//			}
//		}
//	}
//}


