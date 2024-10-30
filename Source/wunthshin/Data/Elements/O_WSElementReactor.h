// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "O_WSElementReactor.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class WUNTHSHIN_API UO_WSElementReactor : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief 원소 작용이 발생했을때 호출되는 함수
	 * @param InReactionInstigator 작용하기 전 마지막에 짝이 되는 원소를 제공한 대상
	 * @param InReactionAssist 가장 첫번째로 짝이 되는 원소를 적용한 대상
	 * @param InReactionOrigin 작용의 대상이 된 대상
	 */
	virtual void React(AActor* InReactionInstigator, AActor* InReactionAssist, AActor* InReactionOrigin) {};
};
