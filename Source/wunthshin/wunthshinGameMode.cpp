// Copyright Epic Games, Inc. All Rights Reserved.

#include "wunthshinGameMode.h"

#include "wunthshinPlayerController.h"
#include "wunthshinPlayerState.h"
#include "Actors/Pawns/Character/AA_WSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AwunthshinGameMode::AwunthshinGameMode()
{
	DefaultPawnClass = AA_WSCharacter::StaticClass();
	PlayerStateClass = AwunthshinPlayerState::StaticClass();
	PlayerControllerClass = AwunthshinPlayerController::StaticClass();
}

APawn* AwunthshinGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	AA_WSCharacter* NewPawn = GetWorld()->SpawnActorDeferred<AA_WSCharacter>
		(
			AA_WSCharacter::StaticClass(),
			FTransform::Identity,
			NewPlayer,
			GetInstigator(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

	// todo: 플레이어가 선택한 캐릭터로 설정
	// OnConstruction (에셋을 불러오는 시점) 전에 에셋을 지정
	NewPawn->SetAssetName("YinLin");
	NewPawn->FinishSpawning(SpawnTransform);
	return NewPawn;
}
