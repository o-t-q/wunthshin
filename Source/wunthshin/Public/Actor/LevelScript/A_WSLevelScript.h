// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "A_WSLevelScript.generated.h"

class AwunthshinPlayerController;

UCLASS()
class WUNTHSHIN_API AA_WSLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()

	UFUNCTION(Server, Reliable)
	void Server_RequestTakeSnapshot(ULevel*                     InLevel,
	                                               UWorld*                     InWorld,
	                                               AwunthshinPlayerController* InController);
	UFUNCTION()
	void OnLevelChanged(ULevel* InLevel, UWorld* InWorld);
	
	void TakeSnapshotProxy(const ULevel* InLevel, const UWorld* InWorld, const AwunthshinPlayerController* InController );
	
public:
	// Sets default values for this actor's properties
	AA_WSLevelScript();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
