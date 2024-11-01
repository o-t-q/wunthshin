// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSNPCAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AA_WSNPCAIController::AA_WSNPCAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AA_WSNPCAIController::SetBehaviorTree(UBehaviorTree* InAsset)
{
	BehaviorTree = InAsset;
}

void AA_WSNPCAIController::BeginPlay()
{
	// 에셋으로 행동 트리가 있다면 설정
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);

		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
}





