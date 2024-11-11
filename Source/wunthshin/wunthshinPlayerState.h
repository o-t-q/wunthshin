// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/Characters/CharacterContext/CharacterContext.h"

#include "GameFramework/PlayerState.h"
#include "wunthshinPlayerState.generated.h"

class AA_WSCharacter;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AwunthshinPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:

};
