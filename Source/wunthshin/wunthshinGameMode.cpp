// Copyright Epic Games, Inc. All Rights Reserved.

#include "wunthshinGameMode.h"

#include "wunthshinPlayerController.h"
#include "wunthshinPlayerState.h"
#include "Actors/Pawns/Character/AA_WSCharacter.h"
#include "Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"
#include "UObject/ConstructorHelpers.h"

AwunthshinGameMode::AwunthshinGameMode()
{
	DefaultPawnClass = AA_WSCharacter::StaticClass();
	PlayerStateClass = AwunthshinPlayerState::StaticClass();
	PlayerControllerClass = AwunthshinPlayerController::StaticClass();
}

void AwunthshinGameMode::BeginPlay()
{
	Super::BeginPlay();
}

APawn* AwunthshinGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>();
	CharacterSubsystem->LoadCharacterLevelSnapshot();
	
	// 만약 이전 레벨에 선택된 살아있는 캐릭터가 있었다면 해당 캐릭터를 반환
	if (AA_WSCharacter* Character = CharacterSubsystem->GetCurrentCharacter())
	{
		Character->SetActorEnableCollision(true);
		Character->SetActorHiddenInGame(false);
		Character->SetActorTransform(SpawnTransform);
		NewPlayer->Possess(Character);
		return Character;
	}

	// 아니라면 기본 캐릭터를 스폰
	AA_WSCharacter* NewPawn = GetWorld()->SpawnActorDeferred<AA_WSCharacter>
		(
			AA_WSCharacter::StaticClass(),
			FTransform::Identity,
			NewPlayer,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

	// todo: 플레이어가 선택한 캐릭터로 설정
	// OnConstruction (에셋을 불러오는 시점) 전에 에셋을 지정
	NewPawn->SetAssetName("YinLin");
	NewPawn->FinishSpawning(SpawnTransform);
	GetGameInstance()->GetSubsystem<UCharacterSubsystem>()->AddCharacter(NewPawn);
	
	return NewPawn;
}
