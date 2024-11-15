// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_WSCheckAlive.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "wunthshin/wunthshinPlayerState.h"
#include "wunthshin/Controller/NPCAIController/A_WSNPCAIController.h"

UBTDecorator_WSCheckAlive::UBTDecorator_WSCheckAlive()
{
}

bool UBTDecorator_WSCheckAlive::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool ReturnValue = false;

	UObject* PlayerActor = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AA_WSNPCAIController::BBPlayerVariable);

	if (const APawn* Pawn = Cast<APawn>(PlayerActor))
	{
		if (const AwunthshinPlayerState* PlayerState = Cast<AwunthshinPlayerState>(Pawn->GetPlayerState()))
		{
			if (PlayerState->IsAlive())
			{
				ReturnValue = true;
			}
		}
	}

	return ReturnValue;
}

void UBTDecorator_WSCheckAlive::ConditionalFlowAbort(UBehaviorTreeComponent& OwnerComp,
	EBTDecoratorAbortRequest RequestMode) const
{
	return Super::ConditionalFlowAbort(OwnerComp, RequestMode);
}