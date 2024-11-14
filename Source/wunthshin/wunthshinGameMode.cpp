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

	// todo: 캐릭터 추가 테스트 코드
	{
		AA_WSCharacter* Character = GetWorld()->SpawnActorDeferred<AA_WSCharacter>
		(
			AA_WSCharacter::StaticClass(),
			FTransform::Identity,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		Character->SetAssetName("Yeonmu");
		Character->FinishSpawning(FTransform::Identity);
		Character->SetActorEnableCollision(false);
		Character->SetActorHiddenInGame(true);

		UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>();
		CharacterSubsystem->AddCharacter(Character, 1);
	}
}

void AwunthshinGameMode::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GetWorld()->IsGameWorld() || GetWorld()->IsPlayInEditor())
	{
		// 이전 레벨의 캐릭터 스냅샷을 로딩
		if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		{
			CharacterSubsystem->LoadCharacterLevelSnapshot();	
		}
	}
}

APawn* AwunthshinGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>();

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
	return NewPawn;
}
