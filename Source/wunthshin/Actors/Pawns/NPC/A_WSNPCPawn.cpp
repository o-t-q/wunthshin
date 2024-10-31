// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshin/Actors/Pawns/NPC/A_WSNPCPawn.h"

#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Components/Shield/C_WSShield.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "wunthshin/Data/NPCs/NPCStats/NPCStats.h"
#include "wunthshin/Data/NPCs/NPCTableRow/NPCTableRow.h"
#include "wunthshin/Subsystem/Utility.h"
#include "wunthshin/Subsystem/NPCSubsystem/NPCSubsystem.h"

// Sets default values
AA_WSNPCPawn::AA_WSNPCPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Inventory = CreateDefaultSubobject<UC_WSInventory>(TEXT("Inventory"));
	Shield = CreateDefaultSubobject<UC_WSShield>(TEXT("Shield"));
	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));
	RightHandWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightHandWeapon"));
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

	if (TableRow->SkeletalMesh)
	{
		MeshComponent->SetSkeletalMesh(TableRow->SkeletalMesh);
	}

	if (TableRow->AnimInstance)
	{
		MeshComponent->SetAnimInstanceClass(TableRow->AnimInstance);
	}

	if (USubsystem* Subsystem = GetSubsystem())
	{
		const IDataTableQuery* TableQuery = Cast<IDataTableQuery>(Subsystem);
		check(TableQuery);

		if (const FNPCStats* Stats = TableQuery->GetRowValue<FNPCStats>(TableRow->Stats.RowName))
		{
			StatsComponent->InitializeStats(*Stats);
		}
	}
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

