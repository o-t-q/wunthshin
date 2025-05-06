// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AwunthshinSpawnPlayerController.h"

#include "wunthshinPlayerState.h"
#include "Actor/Pawn/AA_WSCharacter.h"
#include "Data/Character/ClientCharacterInfo.h"
#include "Data/Item/WSSharedInventory.h"
#include "Net/UnrealNetwork.h"
#include "Subsystem/CharacterSubsystem.h"

AwunthshinSpawnPlayerController::AwunthshinSpawnPlayerController()
{
	SharedInventory = CreateDefaultSubobject<AWSSharedInventory>( "SharedInventory" );
	SharedInventory->SetNetAddressable();
	SharedInventory->SetReplicates( true );
	SharedInventory->bOnlyRelevantToOwner = true;
}

void AwunthshinSpawnPlayerController::UpdateByAlive( const bool bInbAlive )
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

void AwunthshinSpawnPlayerController::SpawnAsCharacter( uint32 InIndex )
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

void AwunthshinSpawnPlayerController::Server_SpawnAsCharacter_Implementation( uint32 InIndex )
{
	SpawnAsCharacter( InIndex );
}

void AwunthshinSpawnPlayerController::RestartLevel()
{
	Super::RestartLevel();

	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		AClientCharacterInfo* Info = CharacterSubsystem->GetClientInfo( GetPlayerState<AwunthshinPlayerState>()->GetUserID() );
		Info->ResetPlayer();
	}
	
	GetPlayerState<AwunthshinPlayerState>()->SetAlive(true);
}

void AwunthshinSpawnPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GetPlayerState<AwunthshinPlayerState>()->OnPlayerAlivenessChanged.AddUniqueDynamic(this, &AwunthshinSpawnPlayerController::UpdateByAlive);
}

void AwunthshinSpawnPlayerController::OnPossess( APawn* InPawn )
{
	Super::OnPossess( InPawn );

	// 캐릭터의 체력이 다 소모됐을때 캐릭터를 자동으로 교환하기 위한 Delegate
	if (AA_WSCharacter* CharacterCasting = Cast<AA_WSCharacter>(InPawn))
	{
		CharacterCasting->OnTakeAnyDamage.AddUniqueDynamic(GetPlayerState<AwunthshinPlayerState>(), &AwunthshinPlayerState::CheckCharacterDeath);
	}
}

void AwunthshinSpawnPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	
	if (AA_WSCharacter* CharacterCasting = GetPawn<AA_WSCharacter>())
	{
		CharacterCasting->OnTakeAnyDamage.RemoveAll(GetPlayerState<AwunthshinPlayerState>());
	}
}

void AwunthshinSpawnPlayerController::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );
	DOREPLIFETIME_CONDITION( AwunthshinSpawnPlayerController, SharedInventory, COND_OwnerOnly )
}
