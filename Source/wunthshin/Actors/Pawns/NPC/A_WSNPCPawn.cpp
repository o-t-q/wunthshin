// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshin/Actors/Pawns/NPC/A_WSNPCPawn.h"

#include <wunthshin/Controller/NPCAIController/A_WSNPCAIController.h>
#include "Components/CapsuleComponent.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Components/Shield/C_WSShield.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "wunthshin/Data/NPCs/NPCStats/NPCStats.h"
#include "wunthshin/Data/NPCs/NPCTableRow/NPCTableRow.h"
#include "wunthshin/Subsystem/Utility.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/NPC/NPCSubsystem.h"
#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR
#include "wunthshinEditorModule/Subsystem/EditorSubsystem/NPC/NPCEditorSubsystem.h"
#endif
#include "GameFramework/FloatingPawnMovement.h"
#include "wunthshin/Actors/Item/A_WSItem.h"
#include "wunthshin/Actors/Item/Weapon/A_WSWeapon.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Components/Skill/C_WSSkill.h"
#include "wunthshin/Data/Items/DamageEvent/WSDamageEvent.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"
#include "wunthshin/Widgets/DamageCounter/WG_WSDamageCounter.h"

DEFINE_LOG_CATEGORY(LogNPCPawn);

// Sets default values
AA_WSNPCPawn::AA_WSNPCPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MovementComponent = CreateOptionalDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Inventory = CreateDefaultSubobject<UC_WSInventory>(InventoryComponentName);
	Shield = CreateDefaultSubobject<UC_WSShield>(TEXT("Shield"));
	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));
	RightHandWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightHandWeapon"));
	Skill = CreateDefaultSubobject<UC_WSSkill>(TEXT("SkillComponent"));
	
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->InitCapsuleSize(42.f, 96.f);
	CapsuleComponent->SetCollisionProfileName("Pawn");

	MeshComponent->SetupAttachment(CapsuleComponent);
	MeshComponent->SetRelativeRotation({ 0.f, 270.f, 0.f });

	Shield->SetupAttachment(MeshComponent);
	MovementComponent->SetUpdatedComponent(RootComponent);

	AIControllerClass = AA_WSNPCAIController::StaticClass();
	AutoPossessPlayer = EAutoReceiveInput::Type::Disabled;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
	
	DamageCounter = CreateDefaultSubobject<UDamageCounterPool>(TEXT("DamageCounters"));
}

void AA_WSNPCPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	FetchAsset(AssetName);
}

UScriptStruct* AA_WSNPCPawn::GetTableType() const
{
	return FNPCTableRow::StaticStruct();
}

void AA_WSNPCPawn::ApplyAsset(const FTableRowBase* InRowPointer)
{
	if (!InRowPointer)
	{
		return;
	}

	const FNPCTableRow* TableRow = reinterpret_cast<const FNPCTableRow*>(InRowPointer);

	UpdatePawnFromDataTable(TableRow);

	if (UFloatingPawnMovement* FloatingPawnMovement = Cast<UFloatingPawnMovement>(MovementComponent))
	{
		FloatingPawnMovement->MaxSpeed = GetStatsComponent()->GetMovementStats().GetNormalMaxSpeed();
	}
}

UClass* AA_WSNPCPawn::GetSubsystemType() const
{
	return UNPCSubsystem::StaticClass();
}

#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR 
UClass* AA_WSNPCPawn::GetEditorSubsystemType() const
{
	return UNPCEditorSubsystem::StaticClass();
}
#endif

// Called when the game starts or when spawned
void AA_WSNPCPawn::BeginPlay()
{
	Super::BeginPlay();
	
	BLUEPRINT_REFRESH_EDITOR

	ensure(
		RightHandWeapon->AttachToComponent
		(
			MeshComponent,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale
	));

	// 월드에 스폰된 NPC들 리스트에 추가
	if (const UWorld* World = GetWorld())
	{
		if (UWorldStatusSubsystem* WorldStatusSubsystem = World->GetSubsystem<UWorldStatusSubsystem>())
		{
			WorldStatusSubsystem->AddNPCPawn(this);
		}
	}
	
}

void AA_WSNPCPawn::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 월드에 스폰된 NPC들 리스트에서 제거
	if (const UWorld* World = GetWorld())
	{
		if (UWorldStatusSubsystem* WorldStatusSubsystem = World->GetSubsystem<UWorldStatusSubsystem>())
		{
			WorldStatusSubsystem->RemoveNPCPawn(this);
		}
	}
}

void AA_WSNPCPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (const FNPCTableRow* Row = GetDataTableHandle().GetRow<FNPCTableRow>("")) 
	{
		if (AA_WSNPCAIController* AIController = Cast<AA_WSNPCAIController>(GetController());
			Row->bUseAI && AIController)
		{
			if (Row->UserDefinedSensory)
			{
				AIController->SetPerceptionComponent(Row->UserDefinedSensory);
			}
			
			if (Row->BehaviorTree)
			{
				AIController->SetBehaviorTree(Row->BehaviorTree);
			}
		}
	}
}

// Called to bind functionality to input
void AA_WSNPCPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AA_WSNPCPawn::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (FWSDamageEvent const& CustomEvent = reinterpret_cast<FWSDamageEvent const&>(DamageEvent);
		CustomEvent.IsFirstHit(this))
	{
		if (StatsComponent->GetHP() > 0)
		{
			StatsComponent->DecreaseHP(DamageAmount);
			UE_LOG(LogNPCPawn, Warning, TEXT("TakeDamage! : %s did %f with %s to %s"), *EventInstigator->GetName(), DamageAmount, DamageCauser ? *DamageCauser->GetName() : TEXT("null"), *GetName());
			CustomEvent.SetFirstHit(this);
			PlayHitMontage();
            DamageCounter->Allocate(DamageAmount);

			// 무기를 맞았을 경우 무기의 원소 효과를 부여
			if (const AA_WSWeapon* Weapon = Cast<AA_WSWeapon>(DamageCauser))
			{
				ApplyElement(EventInstigator, Weapon->GetElement());
			}

			Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
			return DamageAmount;
		}
	}

	return 0.f;
}

FName AA_WSNPCPawn::GetAssetName() const
{
	return AssetName;
}

UCapsuleComponent* AA_WSNPCPawn::GetCapsuleComponent() const
{
	return CapsuleComponent;
}

USkeletalMeshComponent* AA_WSNPCPawn::GetSkeletalMeshComponent() const
{
	return MeshComponent;
}

IInventoryComponent* AA_WSNPCPawn::GetInventoryComponent() const
{
	return Inventory;
}

UC_WSShield* AA_WSNPCPawn::GetShieldComponent() const
{
	return Shield;
}

UStatsComponent* AA_WSNPCPawn::GetStatsComponent() const
{
	return StatsComponent;
}

UChildActorComponent* AA_WSNPCPawn::GetRightHandComponent() const
{
	return RightHandWeapon;
}

UPawnMovementComponent* AA_WSNPCPawn::GetPawnMovementComponent() const
{
	return APawn::GetMovementComponent(); // == FindComponentByClass...
}

void AA_WSNPCPawn::HandleStaminaDepleted()
{
	// todo: 스태미나가 다 떨어진 경우 뛰기를 멈춤
}

void AA_WSNPCPawn::SetHitMontages(const TArray<UAnimMontage*>& InMontages)
{
	HitMontages = InMontages;
}

void AA_WSNPCPawn::PlayHitMontage()
{
	if (HitMontages.IsEmpty())
	{
		return;
	}
	
	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
	{
		if (UAnimMontage* AnimMontage = HitMontages[HitAnimationIndex])
		{
			AnimInstance->Montage_Play(AnimMontage);
			HitAnimationIndex++;
			HitAnimationIndex %= HitMontages.Num();
		}
	}
}

bool AA_WSNPCPawn::Take(UC_WSPickUp* InTakenComponent)
{
	// 아이템을 저장
	AA_WSItem* Item = Cast<AA_WSItem>(InTakenComponent->GetOwner());
	ensure(Item);
	UE_LOG(LogNPCPawn, Log, TEXT("Pick up item: %s"), *Item->GetName());

	// 손이 비어있고, 무기를 잡으려 할때
	if (const AA_WSWeapon* WeaponCast = Cast<AA_WSWeapon>(Item);
		WeaponCast && !RightHandWeapon->GetChildActor())
	{
		RightHandWeapon->SetChildActorClass(WeaponCast->GetClass());
        
		// 런타임 에셋 설정을 위해 Deferred spawn이 필요함
		RightHandWeapon->CreateChildActor([Item](AActor* InActor) 
			{
				AA_WSItem* GeneratedItem = Cast<AA_WSItem>(InActor);
				// 생성에 사용된 클래스가 Item 상속 클래스가 아닌경우
				check(GeneratedItem);

				GeneratedItem->SetAssetName(Item->GetAssetName());
			});

		// 손에 있는 무기를 주울 수 없도록 pick up component를 비활성화
		if (UC_WSPickUp* PickUpComponent = RightHandWeapon->GetChildActor()->GetComponentByClass<UC_WSPickUp>()) 
		{
			PickUpComponent->SetTaken(this);
		}

		// 충돌 반응 비활성화, overlap으로 반응하는 아이템 프로필로 설정
		RightHandWeapon->GetChildActor()->GetComponentByClass<UMeshComponent>()->SetCollisionProfileName("ItemEquipped");
	}
    
	// 인벤토리로 무기 또는 아이템 저장
	Inventory->AddItem(Item);
	return true;
}

