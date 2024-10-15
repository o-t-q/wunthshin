// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_WSPickUp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, AActor*, InTriggeringActor);
DECLARE_LOG_CATEGORY_EXTERN(LogPickUpComponent, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSPickUp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_WSPickUp();

	// 해당 물체가 줍기를 당할 경우 해당 Delegate를 Broadcast
	FOnPickUp OnPickUp;

	// 충돌 처리 활성화
	void EnablePickUp();

	// 충돌 처리 비활성화
	void DisablePickUp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnPickUp(AActor* InTriggeredActor);

private:
	// OnActorBeginOverlap을 OnPickUp에 Bind하기 위한 프록시 함수
	UFUNCTION()
	void OnActorBeginOverlapProxy(AActor* OverlappedActor, AActor* OtherActor);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
