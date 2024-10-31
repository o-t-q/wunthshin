// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"
#include "wunthshin/Interfaces/ElementTracked/ElementTracked.h"

#include "A_WSNPCPawn.generated.h"

class UStatsComponent;
class UC_WSShield;
class UC_WSInventory;

UCLASS()
class WUNTHSHIN_API AA_WSNPCPawn : public APawn, public IDataTableFetcher, public IElementTracked
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSInventory* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSShield* Shield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UStatsComponent* StatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* RightHandWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	FName AssetName;
	
public:
	// Sets default values for this pawn's properties
	AA_WSNPCPawn();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual UScriptStruct* GetTableType() const override;

	virtual void ApplyAsset(const FTableRowBase* InRowPointer) override;

	virtual UClass* GetSubsystemType() const override;
#ifdef WITH_EDITOR
	virtual UClass* GetEditorSubsystemType() const override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 사용할 에셋의 이름
	void SetAssetName(const FName& InAssetName) { AssetName = InAssetName; }

};
