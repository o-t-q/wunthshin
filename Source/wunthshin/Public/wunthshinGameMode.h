// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "wunthshinGameMode.generated.h"

UCLASS(minimalapi)
class AwunthshinGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AwunthshinGameMode();

	virtual void BeginPlay() override;

	virtual void Logout(AController* Exiting) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
};



