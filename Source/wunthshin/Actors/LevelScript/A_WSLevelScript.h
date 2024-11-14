// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "A_WSLevelScript.generated.h"

UCLASS()
class WUNTHSHIN_API AA_WSLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()

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
