// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshin/AnimNotify/FirstAtteckAnimNotifyState.h"
#include "Engine/DamageEvents.h"
#include "wunthshin/Actors/Item/Weapon/A_WSWeapon.h"
#include "wunthshin/Components/Weapon/C_WSWeapon.h"
#include "wunthshin/Data/Items/DamageEvent/WSDamageEvent.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"


UFirstAtteckAnimNotifyState::UFirstAtteckAnimNotifyState()
{
}

void UFirstAtteckAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UFirstAtteckAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
	TArray<APawn*> Exclude;
	Exclude.Add(Self); // 본인

	// 폰의 자식 액터들을 가져와 (무기가 있다는 가정 하에서)
	TArray<AActor*> ChildActors;
	Self->GetAllChildActors(ChildActors);

	for (AActor* ChildActor : ChildActors) 
	{
		const UC_WSWeapon* WeaponComponent = ChildActor->GetComponentByClass<UC_WSWeapon>();

		// 무기 컴포넌트가 없어서 무기가 아닌걸로
		if (!WeaponComponent) 
		{
			continue;
		}

		AA_WSWeapon* Weapon = Cast<AA_WSWeapon>(WeaponComponent->GetOwner());
		check(Weapon);
		const UMeshComponent* WeaponMesh = WeaponComponent->GetOwner()->GetComponentByClass<UMeshComponent>();

		// 무기가 Overlap event를 생성하는지 확인
		check(WeaponMesh->GetGenerateOverlapEvents());

		// 무기와 충돌한 대상들을 찾고
		const TArray<FOverlapInfo>& OverlapInfos = WeaponMesh->GetOverlapInfos();

		// 무기 고유의 데미지 이벤트를 생성한 후 
		FWSDamageEvent DamageEvent{};
		if (const UWorldStatusSubsystem* Subsystem = Weapon->GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
		{
			DamageEvent.SetDamageEvent(Subsystem->GetCurrentAttackID(WeaponComponent));
		}

		for (const FOverlapInfo& OverlapInfo : OverlapInfos)
		{
			// 제어가 가능한 대상이며 (Possessable)
			if (APawn* HitActor = Cast<APawn>(OverlapInfo.OverlapInfo.GetActor()))
			{
				// 예외대상이 아닌
				if (Exclude.Contains(HitActor))
				{
					continue;
				}

#ifdef WITH_EDITOR
				DrawDebugDirectionalArrow
				(
					Weapon->GetWorld(), // Notify의 월드 대신 무기/액터의 월드로
					Weapon->GetTargetLocation(HitActor),
					HitActor->GetActorLocation(),
					2.f,
					FColor::Red,
					false,
					2.f
				);
#endif
				
				// 데미지를 가한다
				HitActor->TakeDamage(WeaponComponent->GetDamage(), DamageEvent, Self->GetController(), Weapon);
			}
		}
	}
	
	/*
	//// 이동 중 매 프레임마다 위치를 변경합니다. 지금 필요한 기능은 아님
	//FVector NewLocation = MeshComp->GetOwner()->GetActorLocation() + FVector(0.f, 200.f * FrameDeltaTime, 0.f); // y축으로 이동
	//MeshComp->GetOwner()->SetActorLocation(NewLocation);
	

	//const FVector WeaponSoketLocation = MeshComp->GetSocketLocation(TEXT("RightHandWepon"));// 무기 컴포넌트 이름 넣어야함
	//FHitResult HitResult;
	//
	//
	//TArray<AActor*> IgnoreActors;
	//// 오너에 따른 엑터에 따라 적과 플레이어를 설정하여 FName("BlockAll"),이부분을 수정하면 된다.
	//AActor* OwningActor = MeshComp->GetOwner();



	// 콜리전 네임은 나중에 적과 다른플레이어를 구분하는 콜리전 생성시 변경해야함
	//  RightHandWeapon의 소켓의 위치에 따른 충돌을 탐치하여 인식함
	if (UKismetSystemLibrary::SphereTraceSingleByProfile(MeshComp,
		WeaponSoketLocation,
		WeaponSoketLocation,
		50,
		FName("BlockAll"),
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,// 디버그 보고싶지 않으면 None으로 변경
		HitResult,
		true,FLinearColor::Red, FLinearColor::Green,5.f))
	{
		AActor* DamagedActor = HitResult.GetActor();
		// 여기에 케릭터 스테이트의 공격력을 바탕으로 데미지를 넣으면 됨
	}*/
}

void UFirstAtteckAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

