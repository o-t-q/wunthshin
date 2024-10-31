// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshin/Actors/Pawns/NPC/A_WSNPCPawn.h"
#include "wunthshin/Data/NPCs/NPCTableRow/NPCTableRow.h"
#include "wunthshin/Subsystem/NPCSubsystem/NPCSubsystem.h"

// Sets default values
AA_WSNPCPawn::AA_WSNPCPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

UScriptStruct* AA_WSNPCPawn::GetTableType() const
{
	return FNPCTableRow::StaticStruct();
}

void AA_WSNPCPawn::ApplyAsset(const FDataTableRowHandle& InRowHandle)
{
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

// Called every frame
void AA_WSNPCPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AA_WSNPCPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

