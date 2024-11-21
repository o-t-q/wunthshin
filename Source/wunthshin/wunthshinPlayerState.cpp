// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshinPlayerState.h"

#include "Actors/Pawns/Character/AA_WSCharacter.h"
#include "Components/Stats/StatsComponent.h"
#include "Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"
#include "Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"

void AwunthshinPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (UWorldStatusSubsystem* WorldStatusSubsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>();
		WorldStatusSubsystem && Cast<AA_WSCharacter>(GetPawn()))
	{
		OnPlayerAlivenessChanged.AddUniqueDynamic(WorldStatusSubsystem, &UWorldStatusSubsystem::PlayDeathLevelSequence); 
	}
}

void AwunthshinPlayerState::SetAlive(const bool InbAlive)
{
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
	if (const ICommonPawn* CommonPawn = Cast<ICommonPawn>(DamagedActor))
	{
		if (CommonPawn->GetStatsComponent()->GetHP() == 0)
		{
			if (Cast<AA_WSCharacter>(CommonPawn))
			{
				UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>();
				if (const int32 Index = CharacterSubsystem->GetAvailableCharacter();
					Index != -1)
				{
					CharacterSubsystem->SpawnAsCharacter(Index);
					SetAlive(true);
					return;
				}
			}
			
			SetAlive(false);
		}
	}
}
