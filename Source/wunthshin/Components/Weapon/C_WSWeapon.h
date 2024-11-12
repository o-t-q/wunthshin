// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../Source/wunthshin/AnimInstance/BaseAnimInstance.h"

#include "C_WSWeapon.generated.h"

class UInputMappingContext;
struct FEnhancedInputActionEventBinding;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSWeapon : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float DamageModifier = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float AttackSpeed = 1.f;
	
	UFUNCTION()
	void UpdateCache(TScriptInterface<I_WSTaker> InTaker);
	
	void SetupInputComponent();

	// 단일 공격에 대한 다중 공격 처리 방지를 위한 함수
	// WorldStatusSubsystem에 의해 처리됨
	UFUNCTION()
	void PushAttackToWorldStatus() const;
	UFUNCTION()
	void PopAttackFromWorldStatus(UAnimMontage* InMontage, bool bInterrupted);
	
public:
	// Sets default values for this component's properties
	UC_WSWeapon();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION()
	virtual bool AttackDefault();
	UFUNCTION()
	virtual bool AttackSecondary();

	void ResetCounter();
	virtual void BeginDestroy() override;

	bool IsAttackInProgress() const;

	float GetDamage() const { return Damage * DamageModifier; }
	void SetDamage(const float InDamage) { Damage = InDamage; }
	// 공격 속도 배수, 1이면 기본 속도
	void SetAttackSpeed(const float InAttackSpeed) { AttackSpeed = InAttackSpeed; }
	void ResetAttackSpeed() { AttackSpeed = 1.f; }
	// 공격 데미지 배수, 1이면 기본 데미지
	void SetDamageModifier(const float InDamageModifier) { DamageModifier = InDamageModifier; }
	void ResetDamageModifier() { DamageModifier = 1.f; }

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
	int32 NextAttackIndex = 0;
	FTimerHandle ResetCounterTimerHandle;
	float ResetTime = 2.0f;
	
};
