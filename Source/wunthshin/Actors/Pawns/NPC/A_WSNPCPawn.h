// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"
#include "wunthshin/Interfaces/ElementTracked/ElementTracked.h"

#include "A_WSNPCPawn.generated.h"

UCLASS()
class WUNTHSHIN_API AA_WSNPCPawn : public APawn, public IDataTableFetcher, public IElementTracked
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AA_WSNPCPawn();

	virtual UScriptStruct* GetTableType() const override;

	virtual void ApplyAsset(const FDataTableRowHandle& InRowHandle) override;

	virtual UClass* GetSubsystemType() const override;
#ifdef WITH_EDITOR
	virtual UClass* GetEditorSubsystemType() const override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
