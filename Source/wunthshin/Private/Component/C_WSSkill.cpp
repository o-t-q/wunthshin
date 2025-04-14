// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/C_WSSkill.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/Pawn/AA_WSCharacter.h"
#include "Component/StatsComponent.h"
#include "Data/Skill/SkillTableRow.h"

DEFINE_LOG_CATEGORY(LogSkillComponent);

// Sets default values for this component's properties
UC_WSSkill::UC_WSSkill()
	: bActive(false), SkillKeyBinding(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Skill
		(
		 TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Skill.IA_Skill'")
		); IA_Skill.Succeeded()) { SkillAction = IA_Skill.Object; }

	if (static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Skill
		(
		 TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ThirdPerson/Input/IMC_Skill.IMC_Skill'")
		); IMC_Skill.Succeeded()) { SkillMappingContext = IMC_Skill.Object; }

	// ...
}


// Called when the game starts
void UC_WSSkill::BeginPlay()
{
	Super::BeginPlay();
	InitializeInputComponent();
	
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UC_WSSkill::ReconfigureInputComponent);
	}
	
	// ...
	
}

void UC_WSSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->OnPossessedPawnChanged.RemoveAll(this);
	}
}

void UC_WSSkill::CastSkill()
{
	if (GetSkillActive())
	{
		return;
	}
	
	if (ICommonPawn* CommonPawn = Cast<ICommonPawn>(GetOwner()))
	{
		const FSkillTableRow* SkillDescription = CharacterSkill.Handle.GetRow<FSkillTableRow>(TEXT(""));
		if (!SkillDescription)
		{
			UE_LOG(LogSkillComponent, Log, TEXT("No skill has been bind to this character"));
			return;
		}
		
		AActor* Actor = Cast<AActor>(CommonPawn);
		UStatsComponent* StatsComponent = CommonPawn->GetStatsComponent();
		
		if (StatsComponent->GetStamina() < SkillDescription->Parameter.StaminaCost)
		{
			return;
		}

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
				bool bSkill = SkillCast->CastSkill(CharacterSkill, HitResult.Location, HitResult.GetActor());

				if (bSkill)
				{
					StatsComponent->DecreaseStamina(SkillDescription->Parameter.StaminaCost);
				}
				SetSkillActive(bSkill);
			}
		}
		else
		{
			// 캐릭터의 위치를 반환, 나머지 스킬의 속성에 따라 프로세서가 처리
			// e.g., 캐릭터의 주변에 버프, 캐릭터가 바라보는 방향에 스킬 등...
			UE_LOG(LogSkillComponent, Log, TEXT("CastSkill: Non-Targeting %s"), *Actor->GetActorLocation().ToString());
			ISkillCast* SkillCast = Cast<ISkillCast>(CommonPawn);
			bool bSkill = SkillCast->CastSkill(CharacterSkill, Actor->GetActorLocation(), nullptr);

			if (bSkill)
			{
				StatsComponent->DecreaseStamina(SkillDescription->Parameter.StaminaCost);
			}
			SetSkillActive(bSkill);
		}
	}
}

void UC_WSSkill::InitializeInputComponent()
{
	if (const APawn* Owner = Cast<APawn>(GetOwner()))
	{
		if (const AController* Controller = Owner->GetController();
			Controller && !Controller->IsA<APlayerController>())
		{
			return;
		}
	}
	
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
			!Subsystem->HasMappingContext(SkillMappingContext))
		{
			Subsystem->AddMappingContext(SkillMappingContext, 0);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetOwner()->InputComponent))
		{
			UE_LOG(LogSkillComponent, Log, TEXT("Binding the skill input"));
			EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Started, this, &UC_WSSkill::CastSkill);
		}
	}
}

void UC_WSSkill::ReconfigureInputComponent(APawn* OldPawn, APawn* NewPawn)
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	if (NewPawn)
	{
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(NewPawn->InputComponent);
		ensure(EnhancedInputComponent);

		if (OldPawn)
		{
			UE_LOG(LogSkillComponent, Log, TEXT("Unbinding skill input"));
			EnhancedInputComponent->ClearBindingsForObject(this);
		}
		
		if (GetOwner() == NewPawn)
		{
			UE_LOG(LogSkillComponent, Log, TEXT("Binding the skill input"));
			EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Started, this, &UC_WSSkill::CastSkill);
		}
	}
}
