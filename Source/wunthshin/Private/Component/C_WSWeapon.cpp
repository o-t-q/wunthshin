// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/C_WSWeapon.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Component/C_WSPickUp.h"
#include "Actor/Item/A_WSWeapon.h"
#include "Interface/Taker.h"
#include "Subsystem/CharacterSubsystem.h"
#include "Subsystem/WorldStatusSubsystem.h"


// Sets default values for this component's properties
UC_WSWeapon::UC_WSWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> Asset
	{ TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ThirdPerson/Input/IMC_SwordAtteck.IMC_SwordAtteck'") };
	check(Asset.Object);
	IMC_Weapon = Asset.Object;
	

	// ...
}


// Called when the game starts
void UC_WSWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (UC_WSPickUp* PickUpComponent = GetOwner()->GetComponentByClass<UC_WSPickUp>()) 
	{
		PickUpComponent->OnPickUpSuccess.AddUniqueDynamic(this, &UC_WSWeapon::UpdateCache);
	}

	if (AA_WSWeapon* WeaponCasting = Cast<AA_WSWeapon>(GetOwner())) 
	{
		AttackMontages = WeaponCasting->GetAttackMontages();
	}

	if (AActor* Taken = GetOwner()->GetAttachParentActor())
	{
		UpdateCache(Taken);
	}

	// ...
	
}

void UC_WSWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (OwningPawn) 
	{
		if (const UCharacterSubsystem* CharacterSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCharacterSubsystem>();
			CharacterSubsystem && CharacterSubsystem->IsOwnedCharacter(Cast<AA_WSCharacter>(OwningPawn)))
		{
			if (AActor* Weapon = Cast<AActor>(GetOwner());
				Weapon && Weapon->InputComponent)
			{
				UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Weapon->InputComponent);
				ensure(EnhancedInputComponent);

				EnhancedInputComponent->ClearBindingsForObject(this);
				Weapon->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			}
		}
	}
}

bool UC_WSWeapon::AttackDefault()
{
	if (AttackMontages[NextAttackIndex]) 
	{
		if (!BasicAnimInstance->Montage_IsPlaying(nullptr)) 
		{
			if (const UWorldStatusSubsystem* WorldStatusSubsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
			{
				if (const FGuid& AttackID = WorldStatusSubsystem->GetCurrentAttackID(this);
					AttackID.IsValid())
				{
					PopAttackFromWorldStatus(nullptr, false);
				}
			}
			
			PushAttackToWorldStatus();
			BasicAnimInstance->Montage_Play(AttackMontages[NextAttackIndex], WeaponContext.GetAttackSpeed());
			NextAttackIndex = (NextAttackIndex + 1) % AttackMontages.Num();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	OwningPawn->GetWorldTimerManager().ClearTimer(ResetCounterTimerHandle);
	OwningPawn->GetWorldTimerManager().SetTimer(ResetCounterTimerHandle, this, &UC_WSWeapon::ResetCounter, ResetTime, false);

	return true;
}

void UC_WSWeapon::UpdateCache(TScriptInterface<I_WSTaker> InTaker)
{
	OwningPawn = Cast<APawn>(GetOwner()->GetAttachParentActor());
	check(OwningPawn);
	check(InTaker == OwningPawn);

	USkeletalMeshComponent* MeshComponent = OwningPawn->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComponent)
	{
		check(false);
		return;
	}

	if (BasicAnimInstance)
	{
		BasicAnimInstance->OnMontageEnded.RemoveAll(this);
	}

	BasicAnimInstance = Cast<UBaseAnimInstance>(MeshComponent->GetAnimInstance());
	check(BasicAnimInstance);

	if (BasicAnimInstance)
	{
		BasicAnimInstance->OnMontageEnded.AddUniqueDynamic(this, &UC_WSWeapon::PopAttackFromWorldStatus);
	}
	
	SetupInputComponent();
}

void UC_WSWeapon::SetupInputComponent()
{
	if (const UCharacterSubsystem* CharacterSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCharacterSubsystem>();
		CharacterSubsystem && CharacterSubsystem->IsOwnedCharacter(Cast<AA_WSCharacter>(OwningPawn)))
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
			!Subsystem->HasMappingContext(IMC_Weapon))
		{
			Subsystem->AddMappingContext(IMC_Weapon, 0);
		}

		AActor* hi = Cast<AActor>(GetOwner());

		if (!hi->InputComponent)
		{
			hi->EnableInput(PC);

			UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(hi->InputComponent);
			ensure(EnhancedInputComponent);

			const UInputAction* InputAction = nullptr;
			const TArray<FEnhancedActionKeyMapping>& Mappings = IMC_Weapon->GetMappings();
			for (const FEnhancedActionKeyMapping& It : Mappings)
			{
				if (It.Action->GetFName() == TEXT("IA_Attack"))
				{
					InputAction = It.Action.Get();
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, "AttackDefault");
					break;
				}
			}
		}
	}
}

void UC_WSWeapon::PushAttackToWorldStatus() const
{
	if (UWorldStatusSubsystem* Subsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
	{
		Subsystem->PushAttack(this);
	}
}

void UC_WSWeapon::PopAttackFromWorldStatus(UAnimMontage* InMontage, bool /*bInterrupted*/)
{
	int32 PreviousIndex = (NextAttackIndex - 1) % AttackMontages.Num();
	if (PreviousIndex < 0)
	{
		PreviousIndex = AttackMontages.Num() + PreviousIndex;
	}
	
	if (!InMontage || InMontage == AttackMontages[PreviousIndex])
	{
		if (UWorldStatusSubsystem* Subsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
		{
			Subsystem->PopAttack(this);
		}
	}
}

void UC_WSWeapon::ResetCounter()
{
	NextAttackIndex = 0;
	UE_LOG(LogTemp, Warning, TEXT("Attack Counter Reset"));

}

bool UC_WSWeapon::IsAttackInProgress() const
{
	if (BasicAnimInstance)
	{
		return BasicAnimInstance->Montage_IsPlaying(AttackMontages[NextAttackIndex]);
	}

	return false;
}

bool UC_WSWeapon::AttackSecondary()
{
	// todo: 몽타주 재생
	return false;
}




