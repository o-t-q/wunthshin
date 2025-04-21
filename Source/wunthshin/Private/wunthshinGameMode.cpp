// Copyright Epic Games, Inc. All Rights Reserved.

#include "wunthshinGameMode.h"

#include "wunthshinPlayerState.h"
#include "Actor/Pawn/AA_WSCharacter.h"
#include "Controller/wunthshinPlayerController.h"
#include "Data/Character/ClientCharacterInfo.h"

#include "Network/Subsystem/WSServerSubsystem.h"

#include "Subsystem/CharacterSubsystem.h"
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

void AwunthshinGameMode::Logout( AController* Exiting )
{
	if ( UWSServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UWSServerSubsystem>() )
	{
		ServerSubsystem->TrySendLogoutRequest();
	}

	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		CharacterSubsystem->DestroyClientInfo( Cast<APlayerController>( Exiting ) );
	}

	Super::Logout( Exiting );
}

APawn* AwunthshinGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		const int32 UserID = NewPlayer->GetPlayerState<AwunthshinPlayerState>()->GetUserID();
		AClientCharacterInfo* ClientInfo = CharacterSubsystem->GetClientInfo( UserID );

		if ( !ClientInfo )
		{
			ClientInfo = CharacterSubsystem->InitializeClientInfo( Cast<APlayerController>( NewPlayer ), UserID );
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
		NewPawn->SetAssetName( "YinLin" );
		NewPawn->FinishSpawning( SpawnTransform );
		ClientInfo->AddCharacter( NewPawn );
		
		return NewPawn;
	}

	check( false );
	return nullptr;
}
