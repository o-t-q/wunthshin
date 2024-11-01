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

// Sets default values
AA_WSNPCPawn::AA_WSNPCPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
	
}

// Called to bind functionality to input
void AA_WSNPCPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
	return APawn::GetMovementComponent();
}

void AA_WSNPCPawn::HandleStaminaDepleted()
{
	// todo: 스태미나가 다 떨어진 경우 뛰기를 멈춤
}

