// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LevelScript/A_WSLevelScript.h"

#include "Controller/AwunthshinSpawnPlayerController.h"
#include "Data/Character/ClientCharacterInfo.h"
#include "Streaming/LevelStreamingDelegates.h"

#include "Subsystem/CharacterSubsystem.h"
#include "WorldPartition/WorldPartition.h"

void AA_WSLevelScript::OnLevelChanged(UWorld* InWorld, const ULevelStreaming* InStreamingLevel,  ULevel* InLevel,
                                      ELevelStreamingState InPreviousState, ELevelStreamingState InNewState)
{
	if (InPreviousState == ELevelStreamingState::Unloaded && InNewState == ELevelStreamingState::LoadedVisible)
	{
		if (GetNetMode() == NM_Client)
		{
			// 클라이언트일 경우 RPC를 사용
			Server_RequestTakeSnapshot(InWorld, InStreamingLevel, InLevel, (ELevelStreamingStateBP)InPreviousState, (ELevelStreamingStateBP)InNewState, GetWorld()->GetFirstPlayerController<AwunthshinPlayerController>());
		}
		else if (GetNetMode() == NM_ListenServer)
		{
			// 리스너일 경우 직접 호출
			TakeSnapshotProxy(InWorld, InStreamingLevel, InLevel, InPreviousState, InNewState, GetWorld()->GetFirstPlayerController<AwunthshinPlayerController>());
		}
		else
		{
			// 서버일 경우 아무것도 하지 않음
			return;
		}
	}
}

void AA_WSLevelScript::TakeSnapshotProxy(const UWorld* InWorld, const ULevelStreaming* InStreamingLevel, ULevel* InLevel,
                                         ELevelStreamingState InPreviousState, ELevelStreamingState InNewState,
                                         const AwunthshinPlayerController* InPlayerController)
{
	// 언로드 상태에서 로드 상태로 변경된 경우 (플레이어가 위치를 변경했을 경우로 가정)
	if (InPreviousState == ELevelStreamingState::Unloaded && InNewState == ELevelStreamingState::LoadedVisible)
	{
		if (const UCharacterSubsystem* CharacterSubsystem = InWorld->GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		{
			if (InPlayerController)
			{
				CharacterSubsystem->GetClientInfo( InPlayerController )->TakeCharacterLevelSnapshot();	
			}
		}
	}
}

// Sets default values
AA_WSLevelScript::AA_WSLevelScript()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AA_WSLevelScript::FlushStreamingStateHandlers()
{
	FStreamingStateTuple Tuple;
	while ( StateChangeCache.Dequeue( Tuple ) )
	{
		StreamingStateHandler.Broadcast(Tuple.Get<0>(), Tuple.Get<1>(), Tuple.Get<2>(), (ELevelStreamingStateBP)Tuple.Get<3>(), (ELevelStreamingStateBP)Tuple.Get<4>());
	}

	StreamingStateHandler.Clear();
}

// Called when the game starts or when spawned
void AA_WSLevelScript::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() == NM_Client || GetNetMode() == NM_ListenServer)
	{
		// 클라이언트와 리슨 서버에서 스트리밍 상태가 변경이 될 경우 플레이어의 위치가 이동된다고 가정하고
		// 플레이어의 캐릭터들에 대한 정보를 서버에서 업데이트
		FLevelStreamingDelegates::OnLevelStreamingStateChanged.AddUObject( this, &AA_WSLevelScript::OnLevelChanged );
	}
	
	FLevelStreamingDelegates::OnLevelStreamingStateChanged.AddUObject( this, &AA_WSLevelScript::OnStreamingStateChanged );
}

void AA_WSLevelScript::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FlushStreamingStateHandlers();
	Super::EndPlay(EndPlayReason);
	FLevelStreamingDelegates::OnLevelStreamingStateChanged.RemoveAll( this );	
}

void AA_WSLevelScript::OnStreamingStateChanged(UWorld* World, const ULevelStreaming* LevelStreaming, ULevel* Level,
	ELevelStreamingState LevelStreamingState, ELevelStreamingState LevelStreamingState1)
{
	StateChangeCache.Enqueue({World, LevelStreaming, Level, LevelStreamingState, LevelStreamingState1});
}

bool AA_WSLevelScript::Server_RequestTakeSnapshot_Validate(UWorld* InWorld, const ULevelStreaming* InStreamingLevel,
	ULevel* InLevel, ELevelStreamingStateBP InPreviousState, ELevelStreamingStateBP InNewState,
	AwunthshinPlayerController* InController)
{
	return InController->GetLevel()->GetOutermost() == InStreamingLevel->GetPackage();
}

void AA_WSLevelScript::Server_RequestTakeSnapshot_Implementation(UWorld* InWorld,
                                                                 const ULevelStreaming* InStreamingLevel, ULevel* InLevel, ELevelStreamingStateBP InPreviousState,
                                                                 ELevelStreamingStateBP InNewState, AwunthshinPlayerController* InController)
{
	// 클라이언트 -> 서버: 플레이어 컨트롤러를 키로 전달해서 현재 캐릭터들의 정보를 저장하도록 요청
	TakeSnapshotProxy(InWorld, InStreamingLevel, InLevel, (ELevelStreamingState)InPreviousState, (ELevelStreamingState)InNewState, InController);
}

// Called every frame
void AA_WSLevelScript::Tick(float DeltaTime)
{
	FlushStreamingStateHandlers();
	Super::Tick(DeltaTime);
}

