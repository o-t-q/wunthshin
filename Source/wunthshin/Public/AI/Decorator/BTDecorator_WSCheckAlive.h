// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTDecorator_WSCheckAlive.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UBTDecorator_WSCheckAlive : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_WSCheckAlive();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;
	void ConditionalFlowAbort(UBehaviorTreeComponent& OwnerComp, EBTDecoratorAbortRequest RequestMode) const;
};
