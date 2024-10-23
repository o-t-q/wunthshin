// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
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
	
	CharaterComponentRef = Cast<AA_WSCharacter>(Pawn);
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	

	if (!MovementComponent) { return; }

	const FVector& Velocity = CharaterComponentRef->GetMovementComponent()->Velocity;
	const double MaxSpeed = CharaterComponentRef->GetMovementComponent()->GetMaxSpeed();

	// 닷 프로덕트 -> 앞 우의 길이 -> 앱솔루트 (+ 음수영역인 뒤, 좌를 접음)
	const double Horizontal = FMath::Abs(Velocity.Dot(FVector::ForwardVector + FVector::RightVector));
	const double Vertical = FMath::Abs(Velocity.Dot(FVector::UpVector));

	HorizontalSpeed = FMath::Clamp(Horizontal / MaxSpeed, 0.f, 1.f);
	VerticalSpeed = FMath::Clamp(Vertical / MaxSpeed, 0.f, 1.f);
	bShoudFastRun = CharaterComponentRef->IsFastRunning();
	bShoudWalk = CharaterComponentRef->IsWalking();
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


