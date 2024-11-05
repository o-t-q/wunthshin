// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSNPCAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "wunthshin/Components/BlueprintAIPerception/BlueprintAIPerceptionComponent.h"

const FName AA_WSNPCAIController::BBPlayerVariable("PlayerActor");

AA_WSNPCAIController::AA_WSNPCAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AA_WSNPCAIController::SetBehaviorTree(UBehaviorTree* InAsset)
{
	BehaviorTree = InAsset;
}

void AA_WSNPCAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor && Stimulus.IsValid())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			BlackboardComponent->SetValueAsObject(BBPlayerVariable, Actor);
		}
		else
		{
			BlackboardComponent->ClearValue(BBPlayerVariable);
		}
	}
}

void AA_WSNPCAIController::SetPerceptionComponent(TSubclassOf<UBlueprintAIPerceptionComponent> ComponentType)
{
	if (PerceptionComponent)
	{
		PerceptionComponent->DestroyComponent();
	}

	PerceptionComponent = NewObject<UAIPerceptionComponent>(this, ComponentType);
	PerceptionComponent->RegisterComponent();
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AA_WSNPCAIController::HandleTargetPerceptionUpdated);
}

void AA_WSNPCAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// 에셋으로 행동 트리가 있다면 설정
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
}





