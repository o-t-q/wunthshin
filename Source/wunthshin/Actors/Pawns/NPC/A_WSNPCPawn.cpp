// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshin/Actors/Pawns/NPC/A_WSNPCPawn.h"

#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Components/Shield/C_WSShield.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "wunthshin/Data/NPCs/NPCStats/NPCStats.h"
#include "wunthshin/Data/NPCs/NPCTableRow/NPCTableRow.h"
#include "wunthshin/Subsystem/Utility.h"
#include "wunthshin/Subsystem/NPCSubsystem/NPCSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <wunthshin/Controller/NPCAIController/A_WSNPCAIController.h>

#include "wunthshin/Data/Items/DamageEvent/WSDamageEvent.h"

DEFINE_LOG_CATEGORY(LogNPCPawn);

// Sets default values
AA_WSNPCPawn::AA_WSNPCPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MovementComponent = CreateOptionalDefaultSubobject<UCharacterMovementComponent>(TEXT("MovementComponent"));
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Inventory = CreateDefaultSubobject<UC_WSInventory>(TEXT("Inventory"));
	Shield = CreateDefaultSubobject<UC_WSShield>(TEXT("Shield"));
	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));
	RightHandWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightHandWeapon"));

	SetRootComponent(CapsuleComponent);
	CapsuleComponent->InitCapsuleSize(42.f, 96.f);

	MeshComponent->SetupAttachment(CapsuleComponent);
	MeshComponent->SetRelativeLocation({ 0.f, 0.f, -96.f });
	MeshComponent->SetRelativeRotation({ 0.f, 270.f, 0.f });

	Shield->SetupAttachment(MeshComponent);
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
}

UClass* AA_WSNPCPawn::GetSubsystemType() const
{
	return UNPCSubsystem::StaticClass();
}

#ifdef WITH_EDITOR
UClass* AA_WSNPCPawn::GetEditorSubsystemType() const
{
	return UNPCEditorSubsystem::StaticClass();
}
#endif

// Called when the game starts or when spawned
void AA_WSNPCPawn::BeginPlay()
{
	Super::BeginPlay();

	// 데이터 테이블을 로드
	// 블루프린트로 생성된 후 배치된 객체의 경우 
	// OnConstruction이 호출되지 않기 때문에 핸들이 존재하지 않을 수 있음
	UpdateDataTable(AssetName);
	
	if (const FNPCTableRow* Row = GetDataTableHandle().GetRow<FNPCTableRow>("")) 
	{
		if (Row->bUseAI)
		{
			AIController = GetWorld()->SpawnActorDeferred<AA_WSNPCAIController>(AA_WSNPCAIController::StaticClass(), FTransform::Identity, this);
			
			if (Row->BehaviorTree)
			{
				AIController->SetBehaviorTree(Row->BehaviorTree);
			}

			AIController->FinishSpawning(FTransform::Identity);

			PossessedBy(AIController);
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
		StatsComponent->DecreaseHP(DamageAmount);
		UE_LOG(LogNPCPawn, Warning, TEXT("TakeDamage! : %s did %f with %s to %s"), *EventInstigator->GetName(), DamageAmount, *DamageCauser->GetName(), *GetName());
		CustomEvent.SetFirstHit(this);
		return DamageAmount;
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

UC_WSInventory* AA_WSNPCPawn::GetInventoryComponent() const
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

UPawnMovementComponent* AA_WSNPCPawn::GetMovementComponent() const
{
	return APawn::GetMovementComponent(); // == FindComponentByClass...
}

void AA_WSNPCPawn::HandleStaminaDepleted()
{
	// todo: 스태미나가 다 떨어진 경우 뛰기를 멈춤
}

