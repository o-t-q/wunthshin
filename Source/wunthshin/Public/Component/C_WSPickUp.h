// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_WSPickUp.generated.h"

class I_WSTaker;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, const TScriptInterface<I_WSTaker>&, InTriggeringActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUpSuccess, const TScriptInterface<I_WSTaker>&, InTriggeringActor);
DECLARE_LOG_CATEGORY_EXTERN(LogPickUpComponent, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSPickUp : public UActorComponent
{
	GENERATED_BODY()

	// OnPickUp을 통해 줍는 객체의 시도가 성공할 경우 true
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Replicated)
	bool bTaken;

public:
	// Sets default values for this component's properties
	UC_WSPickUp();

	// 해당 물체가 줍기를 당할 경우 해당 Delegate를 Broadcast
	UPROPERTY(BlueprintAssignable)
	FOnPickUp OnPickUp;

	UPROPERTY(BlueprintAssignable)
	FOnPickUpSuccess OnPickUpSuccess;

	// 물체의 소유권자가 있는지 확인
	UFUNCTION(BlueprintCallable)
	bool IsTaken() const { return bTaken; }
	
	void SetTaken(const TScriptInterface<I_WSTaker>& InTaker);
	
	virtual void SetActive(bool bNewActive, bool bReset) override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void HandleOnPickUp(const TScriptInterface<I_WSTaker>& InTriggeredActor);
};
