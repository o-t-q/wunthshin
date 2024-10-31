// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../Source/wunthshin/AnimInstance/BaseAnimInstance.h"

#include "C_WSWeapon.generated.h"

class UInputMappingContext;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSWeapon : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float Damage;

	UFUNCTION()
	void UpdateCache(TScriptInterface<I_WSTaker> InTaker);
	void SetupInputComponent();

public:
	// Sets default values for this component's properties
	UC_WSWeapon();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void AttackDefault();
	virtual void AttackSecondary();
	void ResetCounter();
	virtual void BeginDestroy() override;

	float GetDamage() const { return Damage; }
	void SetDamage(const float InDamage) { Damage = InDamage; }

protected: 
	// Owner
	// 이 무기의 소유권자 Pawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APawn* OwningPawn = nullptr;
	
	// 소유권자 Skeletal Mesh에 있는 AnimInstance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBaseAnimInstance* BasicAnimInstance = nullptr;

protected: 
	// Input
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInputMappingContext* IMC_Weapon = nullptr;

	FEnhancedInputActionEventBinding* AttackActionBinding;

	// MTG
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UAnimMontage*> AttackMontages;


	//연속 공격 카운트
	int32 ContinuousAttackCount = 0;
	FTimerHandle ResetCounterTimerHandle;
	float ResetTime = 2.0f;
	
};
