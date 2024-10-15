// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_WSPickUp.generated.h"

class I_WSTaker;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, TScriptInterface<I_WSTaker>, InTriggeringActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDropping, TScriptInterface<I_WSTaker>, InTriggeringActor);
DECLARE_LOG_CATEGORY_EXTERN(LogPickUpComponent, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSPickUp : public UActorComponent
{
	GENERATED_BODY()

	// OnPickUp을 통해 줍는 객체의 시도가 성공할 경우 true
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bTaken;

public:
	// Sets default values for this component's properties
	UC_WSPickUp();

	// 해당 물체가 줍기를 당할 경우 해당 Delegate를 Broadcast
	UPROPERTY(BlueprintAssignable)
	FOnPickUp OnPickUp;

	// 해당 물체를 떨어뜨리려하는 경우 해당 Delegate를 Broadcast
	UPROPERTY(BlueprintAssignable)
	FOnDropping OnDropping;

	// 충돌 처리 활성화
	UFUNCTION(BlueprintCallable)
	void EnablePickUp();

	// 충돌 처리 비활성화
	UFUNCTION(BlueprintCallable)
	void DisablePickUp();

	UFUNCTION(BlueprintCallable)
	bool IsTaken() const { return bTaken; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnPickUp(TScriptInterface<I_WSTaker> InTriggeredActor);

	UFUNCTION()
	void HandleOnDropping(TScriptInterface<I_WSTaker> InTriggeringActor);

private:
	// OnActorBeginOverlap을 OnPickUp에 Bind하기 위한 프록시 함수
	UFUNCTION()
	void OnActorBeginOverlapProxy(AActor* OverlappedActor, AActor* OtherActor);

};
