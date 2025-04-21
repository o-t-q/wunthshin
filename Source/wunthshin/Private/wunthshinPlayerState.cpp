// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshinPlayerState.h"

#include "Actor/Pawn/AA_WSCharacter.h"
#include "Component/StatsComponent.h"
#include "Subsystem/CharacterSubsystem.h"
#include "Subsystem/WorldStatusSubsystem.h"
#include "Data/Character/ClientCharacterInfo.h"

#include "Net/UnrealNetwork.h"

void AwunthshinPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if ( !HasAuthority() )
	{
		if ( UWorldStatusSubsystem* WorldStatusSubsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>();
			 WorldStatusSubsystem && Cast<AA_WSCharacter>(GetPawn()))
		{
			OnPlayerAlivenessChanged.AddUniqueDynamic(WorldStatusSubsystem, &UWorldStatusSubsystem::PlayDeathLevelSequence); 
		}
	}
}

void AwunthshinPlayerState::SetAlive(const bool InbAlive)
{
	if ( !HasAuthority() )
	{
		return;
	}

	const bool TempBool = bAlive;
	bAlive = InbAlive;
	
	if (TempBool != InbAlive)
	{
		OnPlayerAlivenessChanged.Broadcast(bAlive);
	}

	if (APawn* ThisPawn = GetPawn())
	{
		ThisPawn->SetActorEnableCollision(bAlive);
	}
}

void AwunthshinPlayerState::CheckCharacterDeath(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                                AController* InstigatedBy, AActor* DamageCauser)
{
	if ( !HasAuthority() )
	{
		return;
	}

	if ( const TScriptInterface<ICommonPawn> CommonPawn = DamagedActor)
	{
		if ( CommonPawn->GetStatsComponent()->GetHP() == 0 )
		{
			if ( Cast<AA_WSCharacter>( CommonPawn.GetInterface() ) )
			{
				UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>();

				if (AClientCharacterInfo* CharacterInfo = CharacterSubsystem->GetClientInfo( UserID ))
				{
					if (const int32 Index = CharacterInfo->GetAvailableCharacter();
						Index != -1)
					{
						CharacterInfo->SpawnAsCharacter(Index);
						SetAlive(true);
						return;
					}	
				}
			}
			
			SetAlive(false);
		}
	}
}

void AwunthshinPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AwunthshinPlayerState, UserID);
	DOREPLIFETIME(AwunthshinPlayerState, bAlive);
	DOREPLIFETIME(AwunthshinPlayerState, CachedClientCharacter);
}

void AwunthshinPlayerState::SetUserID( const int32 InUserID )
{
	if (!HasAuthority())
	{
		return;
	}

	UserID = InUserID;
}

void AwunthshinPlayerState::OnRep_Alive() const
{
	OnPlayerAlivenessChanged.Broadcast(bAlive);
}
