// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/BaseAnimInstance.h"
#include "Data/Item/WeaponContext.h"

#include "C_WSWeapon.generated.h"

class I_WSTaker;
class UInputMappingContext;
struct FEnhancedInputActionEventBinding;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSWeapon : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	FWeaponContext WeaponContext;
	
	UFUNCTION()
	void UpdateCache( const TScriptInterface<I_WSTaker>& InTaker);
	
	void SetupInputComponent();

	// 단일 공격에 대한 다중 공격 처리 방지를 위한 함수
	// WorldStatusSubsystem에 의해 처리됨
	UFUNCTION()
	void PushAttackToWorldStatus() const;
	UFUNCTION()
	void PopAttackFromWorldStatus( UAnimMontage* InMontage, bool bInterrupted);
	
public:
	// Sets default values for this component's properties
	UC_WSWeapon();

	void SetRespawn(const bool InbRespawn) { bRespawn = InbRespawn; }
	bool IsRespawn() const { return bRespawn;  }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(Server, Reliable)
	void Server_AttackDefault();

	UFUNCTION()
	virtual bool AttackDefault();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_StartAttackMontage();

	UFUNCTION()
	virtual bool AttackSecondary();

	FWeaponContext& GetWeaponContext() { return WeaponContext; }
	const FWeaponContext& GetWeaponContext() const { return WeaponContext; }
	void ResetCounter();
	bool IsAttackInProgress() const;

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

	// MTG
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bRespawn = false;

	//연속 공격 카운트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	int32 NextAttackIndex = 0;

	FTimerHandle ResetCounterTimerHandle;
	float ResetTime = 2.0f;
	
};
