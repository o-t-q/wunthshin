// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSLevelScript.h"

#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"


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
}

void AA_WSLevelScript::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		CharacterSubsystem->TakeCharacterLevelSnapshot();
	}
}

// Called every frame
void AA_WSLevelScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

