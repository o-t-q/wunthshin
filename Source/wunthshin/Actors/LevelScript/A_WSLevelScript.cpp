// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSLevelScript.h"

#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"


void AA_WSLevelScript::TakeSnapshotProxy(ULevel* InLevel, UWorld* InWorld)
{
	// LoadMap일 경우 InLevel이 nullptr, PreLevelRemovedFromWorld Delegate 참조.
	if (!InLevel)
	{
		if (UCharacterSubsystem* CharacterSubsystem = InWorld->GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		{
			CharacterSubsystem->TakeCharacterLevelSnapshot();
		}
	}
}

// Sets default values
AA_WSLevelScript::AA_WSLevelScript()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AA_WSLevelScript::BeginPlay()
{
	Super::BeginPlay();
	// EndPlay에서 TakeCharacterLevelSnapshot을 호출하면 무기를 얻어올 수 없음.
	// LoadMap이 호출되는 과정에서 PlayerController의 Pawn에 대해 Destroy가 호출되고
	// 이에 따라 ChildActorComponent의 ChildActor가 Destroy됨
	FWorldDelegates::PreLevelRemovedFromWorld.AddUObject(this, &AA_WSLevelScript::TakeSnapshotProxy);

	// Streaming Level은 한번에 하나씩만 로딩한다는 전제 하에 사용중
	if (UWorldStatusSubsystem* WorldStatusSubsystem = this->GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
	{
		FString NativeName = GetName();

		int32 NameLength = NativeName.Find(TEXT("_C"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		FString BlueprintName = NativeName.LeftChop(NativeName.Len() - NameLength);

		WorldStatusSubsystem->SetCurrentStreamingLevel(*BlueprintName);
	}
}

void AA_WSLevelScript::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	FWorldDelegates::PreLevelRemovedFromWorld.RemoveAll(this);
}

// Called every frame
void AA_WSLevelScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

