// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "wunthshinPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AwunthshinPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AwunthshinPlayerController();

	UFUNCTION()
	void UpdateByAlive(const bool bInbAlive);

	void SpawnAsCharacter(uint32 InIndex);

	UFUNCTION(Server, Reliable)
	void Server_SpawnAsCharacter(uint32 InIndex);
	
	virtual void RestartLevel() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

};
