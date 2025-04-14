// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Effect/EffectParameter.h"
#include "UObject/NoExportTypes.h"
#include "Data/Effect/EffectTableRow.h"

#include "O_WSBaseEffect.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UO_WSBaseEffect : public UObject
{
	GENERATED_BODY()

public:
	/**
	* @brief 아이템 효과가 발동했을때 호출되는 함수
	* @param InParameter 해당 효과에서 참조할 매개변수
	* @param InInstigator 해당 아이템을 사용한 대상
	* @param InTargetActor 아이템 효과가 적용되는 대상
	*/
	virtual void Effect(const FEffectParameter& InParameter, AActor* InInstigator, AActor* InTargetActor) const {};
};
