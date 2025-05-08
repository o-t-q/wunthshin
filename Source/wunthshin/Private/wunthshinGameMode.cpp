// Copyright Epic Games, Inc. All Rights Reserved.

#include "wunthshinGameMode.h"

#include "wunthshinPlayerState.h"
#include "Actor/Pawn/AA_WSCharacter.h"
#include "Controller/AwunthshinSpawnPlayerController.h"
#include "Controller/wunthshinPlayerController.h"
#include "Data/Character/ClientCharacterInfo.h"

#include "Network/Subsystem/WSServerSubsystem.h"
#include "Streaming/LevelStreamingDelegates.h"

#include "Subsystem/CharacterSubsystem.h"
#include "UObject/ConstructorHelpers.h"

AwunthshinGameMode::AwunthshinGameMode()
{
	DefaultPawnClass = AA_WSCharacter::StaticClass();
	PlayerStateClass = AwunthshinPlayerState::StaticClass();
	PlayerControllerClass = AwunthshinSpawnPlayerController::StaticClass();
}

void AwunthshinGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AwunthshinGameMode::Logout( AController* Exiting )
{
	if (AwunthshinPlayerController* ExitingController = Cast<AwunthshinPlayerController>(Exiting))
	{
		ExitingController->Server_SendLogoutRequest();
	}

	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		CharacterSubsystem->DestroyClientInfo( Cast<APlayerController>( Exiting ) );
	}

	Super::Logout( Exiting );
}

APawn* AwunthshinGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	if ( !Cast<AwunthshinSpawnPlayerController>(NewPlayer) )
	{
		return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
	}
	
	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		const int32 UserID = NewPlayer->GetPlayerState<AwunthshinPlayerState>()->GetUserID();
		AClientCharacterInfo* ClientInfo = CharacterSubsystem->GetClientInfo( UserID );

		if ( !ClientInfo )
		{
			ClientInfo = CharacterSubsystem->InitializeClientInfo( Cast<APlayerController>( NewPlayer ), UserID );
		}

		if (ClientInfo->GetCharacter( 0 ) == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%hs: SPWANING TEST YEONMU"), __FUNCTION__);
			AA_WSCharacter* Yinlin = GetWorld()->SpawnActorDeferred<AA_WSCharacter>
				(
					AA_WSCharacter::StaticClass(),
					FTransform::Identity,
					this,
					nullptr,
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
			Yinlin->SetAssetName("YinLin");
			Yinlin->SetActorEnableCollision(false);
			Yinlin->SetActorHiddenInGame(true);
			Yinlin->FinishSpawning(FTransform::Identity);

			ClientInfo->AddCharacter(Yinlin, 0);
		}
		
		if (ClientInfo->GetCharacter(1) == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%hs: SPWANING TEST YEONMU"), __FUNCTION__);
			AA_WSCharacter* Yeonmu = GetWorld()->SpawnActorDeferred<AA_WSCharacter>
				(
					AA_WSCharacter::StaticClass(),
					FTransform::Identity,
					this,
					nullptr,
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
			Yeonmu->SetAssetName("Yeonmu");
			Yeonmu->SetActorEnableCollision(false);
			Yeonmu->SetActorHiddenInGame(true);
			Yeonmu->FinishSpawning(FTransform::Identity);

			ClientInfo->AddCharacter(Yeonmu, 1);
		}

		ClientInfo->LoadCharacterLevelSnapshot();

		// 만약 이전 레벨에 선택된 살아있는 캐릭터가 있었다면 해당 캐릭터를 반환
		if ( AA_WSCharacter* Character = ClientInfo->GetCurrentCharacter() )
		{
			Character->SetActorEnableCollision(true);
			Character->SetActorHiddenInGame(false);
			Character->SetActorTransform(SpawnTransform);
			NewPlayer->Possess(Character);
			return Character;
		}

		// 아니라면 가장 첫번째에 가까운 캐릭터를 스폰
		const int32 AvailableCharacterIndex = ClientInfo->GetAvailableCharacter();
		if (AA_WSCharacter* AvailableCharacter = ClientInfo->GetCharacter( AvailableCharacterIndex ))
		{
			AvailableCharacter->SetActorEnableCollision(true);
			AvailableCharacter->SetActorHiddenInGame(false);
			AvailableCharacter->SetActorTransform(SpawnTransform);
			NewPlayer->Possess(AvailableCharacter);
			return AvailableCharacter;
		}

		// todo: 캐릭터를 스폰할 수 없으면 사망 안내
		if (AvailableCharacterIndex == INDEX_NONE)
		{
			check(false);
		}
	}

	check( false );
	return nullptr;
}
