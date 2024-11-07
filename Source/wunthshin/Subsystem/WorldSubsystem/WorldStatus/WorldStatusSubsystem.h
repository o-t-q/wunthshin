// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "wunthshin/Components/Weapon/C_WSWeapon.h"
#include "wunthshin/Interfaces/CommonPawn/CommonPawn.h"
#include "WorldStatusSubsystem.generated.h"

class ULevelSequence;
class ALevelSequenceActor;
class ULevelSequencePlayer;
class UC_WSWeapon;
class AA_WSCharacter;
class USG_WSItemMetadata;
class AA_WSItem;

struct FItemTicket 
{
	friend class UWorldStatusSubsystem;

	const USG_WSItemMetadata* Item;
	AActor* Instigator;
	AActor* Target;
	float Rate = -1.f;
	
	bool IsValid() const { return !bDisposed; }

	FTimerHandle& GetTimerHandle() { return TimerHandle; }

private:
	static void ExecuteAndAdjustLifetime(const UWorld* InWorld, FItemTicket& InTicket);

	bool bDisposed = false;

	FTimerHandle TimerHandle;

	uint32 ExecuteCount = 0;
	uint32 MaxExecuteCount = 0;
};

USTRUCT(BlueprintType)
struct FDamageTakenArray
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<const ICommonPawn*> Victims;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAttackEnded, FGuid, InAttackID);

/**
 * 현재 월드의 아이템 적용 및 캐릭터 관련 정보를 추적하는 서브시스템
 */
UCLASS()
class WUNTHSHIN_API UWorldStatusSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sequence", meta = (AllowPrivateAccess = "true"))
	ULevelSequencePlayer* CurrentLevelSequence;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sequence", meta = (AllowPrivateAccess = "true"))
	ALevelSequenceActor* LevelSequenceActor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> ItemsNearbyCharacter;

	// 현재 진행중인 모든 공격들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	TMap<const UC_WSWeapon*, FGuid> AttacksInProgress;

	// 공격이 피해를 준 Pawn들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	TMap<FGuid, FDamageTakenArray> DamageTaken;

	TFunction<void()> OnLevelSequenceEnded;
	TArray<FItemTicket> ItemQueue;

public:
	FOnWeaponAttackEnded OnWeaponAttackEnded;
	
	virtual void Tick(float InDeltaTime) override;

	void PlayLevelSequence(ULevelSequence* InSequence, const TFunction<void()>& OnEndedFunction = {});
	bool IsLevelSequencePlaying() const { return CurrentLevelSequence != nullptr; }
	
	UFUNCTION()
	ULevelSequencePlayer* GetCurrentLevelSequence() const { return CurrentLevelSequence; }

	UFUNCTION()
	void ClearLevelSequence()
	{
		OnLevelSequenceEnded();
		CurrentLevelSequence = nullptr;
		LevelSequenceActor = nullptr;
		OnLevelSequenceEnded = {};
	}

	// 추적할 공격을 추가
	void PushAttack(const UC_WSWeapon* InWeapon)
	{
		const FGuid NewAttack = FGuid::NewGuid();
		AttacksInProgress.Add(InWeapon);
		AttacksInProgress[InWeapon] = NewAttack;
	}
	// 추적이 끝난 공격을 제거
	void PopAttack(const UC_WSWeapon* InWeapon)
	{
		if (AttacksInProgress.Contains(InWeapon))
		{
			const FGuid ID = AttacksInProgress[InWeapon];
			OnWeaponAttackEnded.Broadcast(ID);
			DamageTaken.Remove(ID);
			AttacksInProgress.Remove(InWeapon);
		}
	}

	// 공격 입은 대상을 해당 공격에 매핑
	void SetDamageTaken(const ICommonPawn* InPawn, const FGuid& InDamageTakenBy)
	{
		DamageTaken.Add(InDamageTakenBy);
		DamageTaken[InDamageTakenBy].Victims.Add(InPawn);
	}
	void SetDamageTaken(const ICommonPawn* InPawn, const UC_WSWeapon* InWeapon)
	{
		if (AttacksInProgress.Contains(InWeapon))
		{
			SetDamageTaken(InPawn, AttacksInProgress[InWeapon]);
		}
	}

	bool IsDamageTakenBy(const ICommonPawn* InPawn, const FGuid& InDamageTakenBy) const
	{
		if (DamageTaken.Contains(InDamageTakenBy))
		{
			return DamageTaken[InDamageTakenBy].Victims.Contains(InPawn);	
		}

		return false;
	}
	bool IsDamageTakenBy(const ICommonPawn* InPawn, const UC_WSWeapon* InWeapon) const
	{
		if (AttacksInProgress.Contains(InWeapon))
		{
			return IsDamageTakenBy(InPawn, AttacksInProgress[InWeapon]);
		}

		return false;
	}

	FGuid GetCurrentAttackID(const UC_WSWeapon* InWeapon) const
	{
		if (AttacksInProgress.Contains(InWeapon))
		{
			return AttacksInProgress[InWeapon];
		}

		return {};
	}

	void PushItem(const USG_WSItemMetadata* InItem, AActor* InInstigator, AActor* InTarget);
	void PushItem(FItemTicket InItemTicket);

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UWorldStatusSubsystem, STATGROUP_Tickables);
	}

	UFUNCTION(BlueprintCallable)
	AActor* GetNearestItem() const 
	{
		if (ItemsNearbyCharacter.IsEmpty()) 
		{
			return nullptr;
		}

		return ItemsNearbyCharacter[0];
	}

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetNearestItems() const { return ItemsNearbyCharacter; }

	UFUNCTION(BlueprintCallable)
	bool IsItemNearest(const AActor* InActor) 
	{
		if (ItemsNearbyCharacter.IsEmpty()) 
		{
			return false;
		}

		return ItemsNearbyCharacter[0] == InActor;
	}
	
};
