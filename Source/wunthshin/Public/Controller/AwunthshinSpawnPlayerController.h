// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wunthshinPlayerController.h"
#include "AwunthshinSpawnPlayerController.generated.h"

class AWSSharedInventory;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AwunthshinSpawnPlayerController : public AwunthshinPlayerController
{
	GENERATED_BODY()

public:
	AwunthshinSpawnPlayerController();
	
	UFUNCTION()
	void UpdateByAlive(const bool bInbAlive);

	void SpawnAsCharacter(uint32 InIndex);

	UFUNCTION(Server, Reliable)
	void Server_SpawnAsCharacter(uint32 InIndex);

	virtual void RestartLevel() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION()
	AWSSharedInventory* GetSharedInventory() const { return SharedInventory; }
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Replicated)
	AWSSharedInventory* SharedInventory;
	
};
