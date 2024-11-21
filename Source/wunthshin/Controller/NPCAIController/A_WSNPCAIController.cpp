// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSNPCAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "wunthshin/wunthshinPlayerState.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Actors/Pawns/NPC/A_WSNPCPawn.h"
#include "wunthshin/Components/BlueprintAIPerception/BlueprintAIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/BlockingVolume.h"
#include "GameFramework/Actor.h"  
#include "Components/PrimitiveComponent.h"

const FName AA_WSNPCAIController::BBPlayerVariable("PlayerActor");

AA_WSNPCAIController::AA_WSNPCAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	bWantsPlayerState = true;
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

void AA_WSNPCAIController::PasueAIByAlive(const bool bInbAlive)
{
	if (bInbAlive)
	{
		BehaviorTreeComponent->ResumeLogic("Alive");
	}
	else
	{
		BehaviorTreeComponent->StopLogic("Dead");

		DisableOutBlockingVolumeCollision();
	}
}

void AA_WSNPCAIController::DisableOutBlockingVolumeCollision()
{
	// 레벨에 있는 모든 ABlockingVolume을 찾아서 "OutBlockingVolume"을 찾음
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlockingVolume::StaticClass(), FoundVolumes);

	for (AActor* Actor : FoundVolumes)
	{
		// Label을 비교하여 "OutBlockingVolume"인지 확인
		if (UPrimitiveComponent* VolumeComponent = Actor->FindComponentByTag<UPrimitiveComponent>(TEXT("OutBlockingVolume")))
		{
			VolumeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}


void AA_WSNPCAIController::InitPlayerState()
{
	Super::InitPlayerState();
	GetPlayerState<AwunthshinPlayerState>()->OnPlayerAlivenessChanged.AddUniqueDynamic(this, &AA_WSNPCAIController::PasueAIByAlive);
}

void AA_WSNPCAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AA_WSNPCPawn* NPCPawn = Cast<AA_WSNPCPawn>(InPawn))
	{
		NPCPawn->OnTakeAnyDamage.AddUniqueDynamic(GetPlayerState<AwunthshinPlayerState>(), &AwunthshinPlayerState::CheckCharacterDeath);
	}
}

void AA_WSNPCAIController::OnUnPossess()
{
	Super::OnUnPossess();

	if (AA_WSNPCPawn* NPCPawn = GetPawn<AA_WSNPCPawn>())
	{
		NPCPawn->OnTakeAnyDamage.RemoveAll(GetPlayerState<AwunthshinPlayerState>());
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





