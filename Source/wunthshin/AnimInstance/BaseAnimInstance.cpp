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
		checkf(false, TEXT("UBaisicCharacterAnimInstance�� ����Ϸ��� �������ڰ� Pawn�̿��� �մϴ�."));
		return;
	}
	else if (!Pawn) { return; }

	MovementComponent = Pawn->GetMovementComponent();
	check(MovementComponent);
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!MovementComponent) { return; }

	RunSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
	WalkSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);

	bShoudRun = !FMath::IsNearlyZero(RunSpeed);
	bShoudWalk = !FMath::IsNearlyZero(WalkSpeed);


	bIsCrouch = MovementComponent->IsCrouching();
	bIsFalling = MovementComponent->IsFalling();
}
