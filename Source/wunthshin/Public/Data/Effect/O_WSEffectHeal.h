// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Effect/O_WSBaseEffect.h"

#include "O_WSEffectHeal.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemEffectHeal, Log, All);

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UO_WSEffectHeal : public UO_WSBaseEffect
{
	GENERATED_BODY()

public:
	virtual void Effect(const FEffectParameter& InParameter, AActor* InInstigator, AActor* InTargetActor) const override;
};
