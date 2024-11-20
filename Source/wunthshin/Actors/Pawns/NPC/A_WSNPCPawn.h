// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"
#include "wunthshin/Interfaces/ElementTracked/ElementTracked.h"
#include "wunthshin/Interfaces/CommonPawn/CommonPawn.h"
#include "wunthshin/Interfaces/Taker/Taker.h"

#include "A_WSNPCPawn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNPCPawn, Log, All);

class UCapsuleComponent;
class UStatsComponent;
class UC_WSShield;
class UC_WSInventory;
class AA_WSNPCAIController;

UCLASS()
class WUNTHSHIN_API AA_WSNPCPawn : public APawn, public IElementTracked, public ICommonPawn, public I_WSTaker
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSInventory* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSShield* Shield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UStatsComponent* StatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSSkill* Skill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* RightHandWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	UDamageCounterPool* DamageCounter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	FName AssetName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> HitMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsFastRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsWalking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	int32 HitAnimationIndex;

public:
	// Sets default values for this pawn's properties
	AA_WSNPCPawn();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual UScriptStruct* GetTableType() const override;

	virtual void ApplyAsset(const FTableRowBase* InRowPointer) override;

	virtual UClass* GetSubsystemType() const override;
#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR 
	virtual UClass* GetEditorSubsystemType() const override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	// 사용할 에셋의 이름
	void SetAssetName(const FName& InAssetName) { AssetName = InAssetName; }


	// ICommonPawn을(를) 통해 상속됨
	FName GetAssetName() const override;

	virtual UCapsuleComponent* GetCapsuleComponent() const override;
	virtual USkeletalMeshComponent* GetSkeletalMeshComponent() const override;
	virtual IInventoryComponent* GetInventoryComponent() const override;
	virtual UC_WSShield* GetShieldComponent() const override;
	virtual UStatsComponent* GetStatsComponent() const override;
	virtual UChildActorComponent* GetRightHandComponent() const override;
	virtual UPawnMovementComponent* GetPawnMovementComponent() const override;
	virtual UC_WSSkill* GetSkillComponent() const override { return Skill; }
	virtual UDamageCounterPool* GetDamageCounters() const override { return DamageCounter; }
	
	virtual void HandleStaminaDepleted() override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool IsFastRunning() const override { return bIsFastRunning; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool IsWalking() const override { return bIsWalking; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool CheckClimbState() const override { return false; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool CheckClimbDashState() const override { return false; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual FVector ClimbDashDirection() const override { return FVector::Zero(); }

	
private:
	virtual void SetHitMontages(const TArray<UAnimMontage*>& InMontages) override;

public:
	virtual void PlayHitMontage() override;

	virtual bool Take(UC_WSPickUp* InTakenComponent) override;
};
