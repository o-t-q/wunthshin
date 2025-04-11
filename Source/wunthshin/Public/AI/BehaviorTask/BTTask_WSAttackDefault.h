// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WSAttackDefault.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UBTTask_WSAttackDefault : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Tracking")
	TMap<FGuid, UBehaviorTreeComponent*> AttacksByThisNode;

	UFUNCTION()
	void CheckIfAttackByNode(FGuid InAttackID);
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UBTTask_WSAttackDefault();

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
