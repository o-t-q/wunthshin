// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSSkill.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Data/Skills/SkillTableRow/SkillTableRow.h"

DEFINE_LOG_CATEGORY(LogSkillComponent);

// Sets default values for this component's properties
UC_WSSkill::UC_WSSkill(): SkillKeyBinding(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Skill(
			TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Skill.IA_Skill'"));
		IA_Skill.Succeeded())
	{
		SkillAction = IA_Skill.Object;
	}

	if (static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Skill(
			TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ThirdPerson/Input/IMC_Skill.IMC_Skill'"));
		IMC_Skill.Succeeded())
	{
		SkillMappingContext = IMC_Skill.Object;
	}

	// ...
}


// Called when the game starts
void UC_WSSkill::BeginPlay()
{
	Super::BeginPlay();
	SetupInputComponent();
	// ...
	
}

void UC_WSSkill::BeginDestroy()
{
	Super::BeginDestroy();
	RemoveInputComponent();
}

void UC_WSSkill::CastSkill()
{
	if (ICommonPawn* CommonPawn = Cast<ICommonPawn>(GetOwner()))
	{
		const FSkillTableRow* SkillDescription = CharacterSkill.Handle.GetRow<FSkillTableRow>(TEXT(""));
		AActor* Actor = Cast<AActor>(CommonPawn);

		// 스킬이 타게팅이라면,
		if (SkillDescription->bTargeting)
		{
			FHitResult HitResult;
			FCollisionQueryParams Params{NAME_None, false, Actor};
			
			if (const AActor* ChildActor = CommonPawn->GetRightHandComponent()->GetChildActor())
			{
				Params.AddIgnoredActor(ChildActor);
			}

			// 라인 트레이싱을 한 다음 맞는 대상이 있다면,
			const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel
			(
				HitResult,
				Actor->GetActorLocation(),
				Actor->GetActorLocation() + (Actor->GetActorForwardVector() * 1000.f),
				ECC_GameTraceChannel3,
				Params
			);

			DrawDebugLine
			(
				GetWorld(),
				HitResult.TraceStart,
				HitResult.TraceEnd,
				bBlockingHit ? FColor::Green : FColor::Red,
				false,
				0.5f
			);

			if (bBlockingHit)
			{
				UE_LOG(LogSkillComponent, Log, TEXT("CastSkill: Targeting %s"), *HitResult.GetActor()->GetName());
				ISkillCast* SkillCast = Cast<ISkillCast>(CommonPawn);
				SkillCast->CastSkill(CharacterSkill, HitResult.Location, HitResult.GetActor());
			}
		}
		else
		{
			// 캐릭터의 위치를 반환, 나머지 스킬의 속성에 따라 프로세서가 처리
			// e.g., 캐릭터의 주변에 버프, 캐릭터가 바라보는 방향에 스킬 등...
			UE_LOG(LogSkillComponent, Log, TEXT("CastSkill: Non-Targeting %s"), *Actor->GetActorLocation().ToString());
			ISkillCast* SkillCast = Cast<ISkillCast>(CommonPawn);
			SkillCast->CastSkill(CharacterSkill, Actor->GetActorLocation(), nullptr);	
		}
	}
}

void UC_WSSkill::SetupInputComponent()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	
	if (APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		// 입력은 플레이어만 가능하게
		if (PC->GetPawn() != GetOwner())
		{
			return;
		}

		UE_LOG(LogSkillComponent, Log, TEXT("Binding the skill input"));

		if (const AA_WSCharacter* Character = Cast<AA_WSCharacter>(GetOwner()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
				!Subsystem->HasMappingContext(SkillMappingContext))
			{
				Subsystem->AddMappingContext(SkillMappingContext, 0);
			}

			if (!SkillKeyBinding)
			{
				UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Character->InputComponent);
				ensure(EnhancedInputComponent);

				SkillKeyBinding = &EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Started, this, &UC_WSSkill::CastSkill);
			}
		}
	}
}

void UC_WSSkill::RemoveInputComponent()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	
	if (APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		// 입력은 플레이어만 가능하게
		if (PC->GetPawn() != GetOwner())
		{
			return;
		}

		UE_LOG(LogSkillComponent, Log, TEXT("Unbinding the skill input"));

		if (const AA_WSCharacter* Character = Cast<AA_WSCharacter>(GetOwner()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
				Subsystem->HasMappingContext(SkillMappingContext))
			{
				Subsystem->RemoveMappingContext(SkillMappingContext);
			}

			if (SkillKeyBinding)
			{
				UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Character->InputComponent);
				ensure(EnhancedInputComponent);

				EnhancedInputComponent->RemoveBinding(*SkillKeyBinding);
				SkillKeyBinding = nullptr;
			}
		}
	}
}

