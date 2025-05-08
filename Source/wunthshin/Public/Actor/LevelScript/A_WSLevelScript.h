// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/ExternalMutex.h"
#include "Containers/CircularQueue.h"
#include "Engine/LevelScriptActor.h"
#include "A_WSLevelScript.generated.h"

class AwunthshinPlayerController;

UENUM(BlueprintType)
enum class ELevelStreamingStateBP : uint8
{
	Removed,
	Unloaded,
	FailedToLoad,
	Loading,
	LoadedNotVisible,
	MakingVisible,
	LoadedVisible,
	MakingInvisible
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FStreamingStateHandler, UWorld*, InWorld, const ULevelStreaming*, InStreamingLevel, ULevel*, InLevel, ELevelStreamingStateBP, InPreviousState, ELevelStreamingStateBP, InNewState);

UCLASS()
class WUNTHSHIN_API AA_WSLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
	using OnStreamingStateFunctionSignature = void(UWorld*, const ULevelStreaming*, ULevel*, ELevelStreamingState, ELevelStreamingState);
	
public:
	UPROPERTY(BlueprintAssignable)
	FStreamingStateHandler StreamingStateHandler;
	
	using FTStreamingStateHandlingFunction = TFunction<OnStreamingStateFunctionSignature>; 

private:
	using FStreamingStateTuple = TTuple<UWorld*, const ULevelStreaming*, ULevel*, ELevelStreamingState, ELevelStreamingState>;
	TCircularQueue<FStreamingStateTuple> StateChangeCache{ 1024 };

	void OnStreamingStateChanged(UWorld* World, const ULevelStreaming* LevelStreaming, ULevel* Level, ELevelStreamingState LevelStreamingState, ELevelStreamingState LevelStreamingState1);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestTakeSnapshot(UWorld* InWorld, const ULevelStreaming* InStreamingLevel,  ULevel* InLevel,	ELevelStreamingStateBP InPreviousState, ELevelStreamingStateBP InNewState, AwunthshinPlayerController* InController);

	void OnLevelChanged(UWorld* InWorld, const ULevelStreaming* InStreamingLevel, ULevel* InLevel, ELevelStreamingState InPreviousState, ELevelStreamingState
	                    InNewState);
	
	void TakeSnapshotProxy(const UWorld* InWorld, const ULevelStreaming* InStreamingLevel, ULevel* InLevel, ELevelStreamingState InPreviousState, ELevelStreamingState InNewState, const AwunthshinPlayerController* InPlayerController );
	
public:
	// Sets default values for this actor's properties
	AA_WSLevelScript();

	void FlushStreamingStateHandlers();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
