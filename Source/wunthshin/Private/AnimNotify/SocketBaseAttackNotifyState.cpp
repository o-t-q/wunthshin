// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/SocketBaseAttackNotifyState.h"

// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "Component/C_WSWeapon.h"
#include "Data/Item/WSDamageEvent.h"
#include "Subsystem/WorldStatusSubsystem.h"


USocketBaseAttackNotifyState::USocketBaseAttackNotifyState()
{
}

void USocketBaseAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void USocketBaseAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	// Montage가 Pawn을 대상으로 실행된다고 가정하며 (캐릭터 또는 폰)
	APawn* Self = Cast<APawn>(MeshComp->GetOwner());
	if (!Self)
	{
		return;
	}

	// 예외대상을 처리한 후
	TArray<AActor*> Exclude;
	Exclude.Add(Self); // 본인

	// 폰의 자식 액터들을 가져오고
	TArray<AActor*> ChildActors;
	Self->GetAllChildActors(ChildActors);
	Exclude.Append(ChildActors);

	const ICommonPawn* CommonInterface = Cast<ICommonPawn>(Self);
	if (!CommonInterface)
	{
		return;
	}

	AActor* WeaponActor = CommonInterface->GetRightHandComponent()->GetChildActor();
	if (!WeaponActor)
	{
		return;
	}
	const UC_WSWeapon* WeaponComponent = WeaponActor->GetComponentByClass<UC_WSWeapon>();
	if (!WeaponComponent)
	{
		return;
	}
	
	FCollisionQueryParams CollisionQueryParams(NAME_None, false, nullptr);
	CollisionQueryParams.AddIgnoredActors(Exclude);

	const FVector& AttackPosition = CommonInterface->GetSkeletalMeshComponent()->GetSocketLocation(AttackSocket);
	const FQuat& AttackRotation = CommonInterface->GetSkeletalMeshComponent()->GetSocketQuaternion(AttackSocket);
	
	TArray<FOverlapResult> OutOverlaps;
	bool bBlocking = Self->GetWorld()->OverlapMultiByChannel
	(
		OutOverlaps,
		AttackPosition,
		AttackRotation,
		ECC_Pawn,
		FCollisionShape::MakeSphere(50.f),
		CollisionQueryParams
	);

#ifdef WITH_EDITOR
	DrawDebugSphere
	(
		Self->GetWorld(), // Notify의 월드 대신 무기/액터의 월드로
		AttackPosition,
		50.f,
		16,
		FColor::Red,
		false,
		0.5f
	);
#endif

	// 무기 고유의 데미지 이벤트를 생성한 후 
	FWSDamageEvent DamageEvent{};
	if (const UWorldStatusSubsystem* Subsystem = Self->GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
	{
		DamageEvent.SetDamageEvent(Subsystem->GetCurrentAttackID(WeaponComponent));
	}
	
	for (const FOverlapResult& OverlapResult : OutOverlaps)
	{
		// 데미지를 가한다
		AActor* HitActor = OverlapResult.GetActor();
		HitActor->TakeDamage(WeaponComponent->GetWeaponContext().GetDamage(), DamageEvent, Self->GetController(), WeaponActor);
	}
}

void USocketBaseAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

