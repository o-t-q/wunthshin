﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"
#include "wunthshinPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerAliveChanged, const bool, InbAlive);

class AA_WSCharacter;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AwunthshinPlayerState : public APlayerState
{
	GENERATED_BODY()

	// 생존한 캐릭터가 하나라도 존재하는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	bool bAlive = true;
	
public:
	FOnPlayerAliveChanged OnPlayerAlivenessChanged;
	
	void SetAlive(const bool InbAlive)
	{
		const bool TempBool = bAlive;
		bAlive = InbAlive;
		if (TempBool != InbAlive)
		{
			OnPlayerAlivenessChanged.Broadcast(bAlive);
		}
	}
	bool IsAlive() const { return bAlive; }
	
	UFUNCTION()
	void CheckCharacterDeath(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
