// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshinPlayerController.h"

#include "wunthshin/wunthshinPlayerState.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"

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

void AwunthshinPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GetPlayerState<AwunthshinPlayerState>()->OnPlayerAlivenessChanged.AddUniqueDynamic(this, &AwunthshinPlayerController::UpdateByAlive);

	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		if (CharacterSubsystem->GetCharacter(1) == nullptr)
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

			GetGameInstance()->GetSubsystem<UCharacterSubsystem>()->AddCharacter(Yeonmu, 1);
		}
	}
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
