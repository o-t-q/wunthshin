// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"
#include "wunthshinPlayerState.generated.h"

class AClientCharacterInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnPlayerAliveChanged, const bool, InbAlive );

class AA_WSCharacter;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AwunthshinPlayerState : public APlayerState
{
	GENERATED_BODY()

	friend class AwunthshinPlayerController;
	friend class UCharacterSubsystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Identity", meta = (AllowPrivateAccess = "true"))
	int32 UserID;

	// 생존한 캐릭터가 하나라도 존재하는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_Alive, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	bool bAlive = true;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerAliveChanged OnPlayerAlivenessChanged;

	virtual void BeginPlay() override;
	
	void SetAlive(const bool InbAlive);
	bool IsAlive() const { return bAlive; }
	int32 GetUserID() const { return UserID; }

	UFUNCTION()
	void CheckCharacterDeath(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetUserID(int32 InUserID);

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	AClientCharacterInfo* CachedClientCharacter = nullptr;

	UFUNCTION()
	void OnRep_Alive() const;
};
