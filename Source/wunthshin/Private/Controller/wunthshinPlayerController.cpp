// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/wunthshinPlayerController.h"

#include "wunthshinGameMode.h"
#include "wunthshinPlayerState.h"
#include "Actor/Pawn/AA_WSCharacter.h"

#include "Data/Character/ClientCharacterInfo.h"

#include "Subsystem/CharacterSubsystem.h"

AwunthshinPlayerController::AwunthshinPlayerController()
{
}

void AwunthshinPlayerController::UpdateByAlive(const bool bInbAlive)
{
	StopMovement();
	
	if (!bInbAlive)
	{
		GetPawn()->DisableInput(this);
	}
	else
	{
		GetPawn()->EnableInput(this);
	}
}

void AwunthshinPlayerController::SpawnAsCharacter(uint32 InIndex)
{
	if ( HasAuthority() )
	{
		if ( const UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>() )
		{
			CharacterSubsystem->GetClientInfo( this )->SpawnAsCharacter( InIndex );
		}
	}
	else
	{
		Server_SpawnAsCharacter( InIndex );
	}
}

void AwunthshinPlayerController::Server_SpawnAsCharacter_Implementation(uint32 InIndex)
{
	SpawnAsCharacter( InIndex );
}

void AwunthshinPlayerController::RestartLevel()
{
	Super::RestartLevel();

	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		AClientCharacterInfo* Info = CharacterSubsystem->GetClientInfo( GetPlayerState<AwunthshinPlayerState>()->GetUserID() );
		Info->ResetPlayer();
	}
	
	GetPlayerState<AwunthshinPlayerState>()->SetAlive(true);
}

void AwunthshinPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GetPlayerState<AwunthshinPlayerState>()->OnPlayerAlivenessChanged.AddUniqueDynamic(this, &AwunthshinPlayerController::UpdateByAlive);
}

void AwunthshinPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 캐릭터의 체력이 다 소모됐을때 캐릭터를 자동으로 교환하기 위한 Delegate
	if (AA_WSCharacter* CharacterCasting = Cast<AA_WSCharacter>(InPawn))
	{
		CharacterCasting->OnTakeAnyDamage.AddUniqueDynamic(GetPlayerState<AwunthshinPlayerState>(), &AwunthshinPlayerState::CheckCharacterDeath);
	}
}

void AwunthshinPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	if (AA_WSCharacter* CharacterCasting = GetPawn<AA_WSCharacter>())
	{
		CharacterCasting->OnTakeAnyDamage.RemoveAll(GetPlayerState<AwunthshinPlayerState>());
	}
}
