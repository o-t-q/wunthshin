// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LevelScript/A_WSLevelScript.h"

#include "Data/Character/ClientCharacterInfo.h"

#include "Subsystem/CharacterSubsystem.h"
#include "Subsystem/WorldStatusSubsystem.h"


void AA_WSLevelScript::Server_RequestTakeSnapshot_Implementation( ULevel* InLevel, UWorld* InWorld, AwunthshinPlayerController* InController )
{
	// 클라이언트 -> 서버: 플레이어 컨트롤러를 키로 전달해서 현재 캐릭터들의 정보를 저장하도록 요청
	TakeSnapshotProxy(InLevel, InWorld, InController);
}

void AA_WSLevelScript::OnLevelChanged(ULevel* InLevel, UWorld* InWorld)
{
	if (GetNetMode() == NM_Client)
	{
		// 클라이언트일 경우 RPC를 사용
		Server_RequestTakeSnapshot(InLevel, InWorld, GetWorld()->GetFirstPlayerController<AwunthshinPlayerController>());
	}
	else if (GetNetMode() == NM_ListenServer)
	{
		// 리스너일 경우 직접 호출
		TakeSnapshotProxy(InLevel, InWorld, GetWorld()->GetFirstPlayerController<AwunthshinPlayerController>());
	}
	else
	{
		// 서버일 경우 아무것도 하지 않음
		return;
	}
}

void AA_WSLevelScript::TakeSnapshotProxy(const ULevel* InLevel, const UWorld* InWorld, const AwunthshinPlayerController* InController)
{
	// LoadMap일 경우 InLevel이 nullptr, PreLevelRemovedFromWorld Delegate 참조.
	if ( !InLevel )
	{
		if (UCharacterSubsystem* CharacterSubsystem = InWorld->GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		{
			CharacterSubsystem->GetClientInfo(InController)->TakeCharacterLevelSnapshot();
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

	if (GetNetMode() == NM_Client)
	{
		// EndPlay에서 TakeCharacterLevelSnapshot을 호출하면 무기를 얻어올 수 없음.
		// LoadMap이 호출되는 과정에서 PlayerController의 Pawn에 대해 Destroy가 호출되고
		// 이에 따라 ChildActorComponent의 ChildActor가 Destroy됨
		FWorldDelegates::PreLevelRemovedFromWorld.AddUObject( this, &AA_WSLevelScript::OnLevelChanged );
	}

	if ( HasAuthority() )
	{
		// Streaming Level은 한번에 하나씩만 로딩한다는 전제 하에 사용중
		if ( UWorldStatusSubsystem* WorldStatusSubsystem = this->GetWorld()->GetSubsystem<UWorldStatusSubsystem>() )
		{
			const FString NativeName = GetName();
			const int32 NameLength = NativeName.Find(TEXT("_C"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			const FString BlueprintName = NativeName.LeftChop(NativeName.Len() - NameLength);

			WorldStatusSubsystem->SetCurrentStreamingLevel(*BlueprintName);
		}
	}
}

void AA_WSLevelScript::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetNetMode() == NM_Client)
	{	
		FWorldDelegates::PreLevelRemovedFromWorld.RemoveAll(this);	
	}
}

// Called every frame
void AA_WSLevelScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

