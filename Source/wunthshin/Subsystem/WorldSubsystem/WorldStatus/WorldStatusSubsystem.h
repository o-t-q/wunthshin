// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequenceActor.h"
#include "EventTicket/EventTicket.h"
#include "Subsystems/WorldSubsystem.h"
#include "wunthshin/Components/Weapon/C_WSWeapon.h"
#include "wunthshin/Interfaces/CommonPawn/CommonPawn.h"
#include "wunthshin/Data/Elements/ElementTracking/ElementTracking.h"
#include "WorldStatusSubsystem.generated.h"

struct FItemTicket;
class AA_WSNPCPawn;
class ULevelSequence;
class ALevelSequenceActor;
class ULevelSequencePlayer;
class UC_WSWeapon;
class AA_WSCharacter;
class USG_WSItemMetadata;
class AA_WSItem;

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

	// 레벨 시퀀스 플레이어 (스킬, 씬 등..)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sequence", meta = (AllowPrivateAccess = "true"))
	ULevelSequencePlayer* CurrentLevelSequence;

	// 현재 실행중인 레벨 시퀀스 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sequence", meta = (AllowPrivateAccess = "true"))
	ALevelSequenceActor* LevelSequenceActor;

	// 스킬에 씬에 대상이 되는 폰
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	APawn* SkillVictimPawn;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> ItemsNearbyCharacter;

	// 현재 진행중인 모든 공격들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	TMap<const UObject*, FGuid> AttacksInProgress;

	// 공격이 피해를 준 Pawn들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	TMap<FGuid, FDamageTakenArray> DamageTaken;

	// 월드에 스폰된 NPC Pawn들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	TSet<AA_WSNPCPawn*> NPCPawns;

	UPROPERTY(VisibleAnywhere, Category = "Stack", meta = (AllowPrivateAccess = "true"))
	TMap<UObject*, FElementTrackingMap> ElementTrackingObjects;

	TFunction<void()> OnLevelSequenceEnded;
	TArray<TSharedPtr<FEventTicket>> EventQueue;

public:
	FOnWeaponAttackEnded OnWeaponAttackEnded;

	UWorldStatusSubsystem();
	
	virtual void Tick(float InDeltaTime) override;

	void AddNPCPawn(AA_WSNPCPawn* InNewNPCPawn) { NPCPawns.Add(InNewNPCPawn); }
	void RemoveNPCPawn(const AA_WSNPCPawn* InPawnToRemove) { NPCPawns.Remove(InPawnToRemove); }

	void PlayLevelSequence(ULevelSequence* InSequence, const TFunction<void()>& OnEndedFunction = {});
	bool IsLevelSequencePlaying() const { return CurrentLevelSequence != nullptr; }
	void SetSkillVictimPawn(APawn* InSkillVictimPawn) { SkillVictimPawn = InSkillVictimPawn; }

	UFUNCTION(BlueprintCallable)
	AActor* GetSkillVictimPawn() const { return SkillVictimPawn; }

	UFUNCTION()
	ULevelSequencePlayer* GetCurrentLevelSequence() const { return CurrentLevelSequence; }

	void FreezeSpawnedNPCsBT();
	void UnfreezeSpawnedNPCsBT();

	UFUNCTION()
	void ClearLevelSequence()
	{
		OnLevelSequenceEnded();
		LevelSequenceActor->Destroy();
		CurrentLevelSequence = nullptr;
		LevelSequenceActor = nullptr;
		SetSkillVictimPawn(nullptr);
		OnLevelSequenceEnded = {};
	}

	// 추적할 공격을 추가
	void PushAttack(const UObject* InGiver)
	{
		const FGuid NewAttack = FGuid::NewGuid();
		AttacksInProgress.Add(InGiver);
		AttacksInProgress[InGiver] = NewAttack;
	}
	// 추적이 끝난 공격을 제거
	void PopAttack(const UObject* InGiver)
	{
		if (AttacksInProgress.Contains(InGiver))
		{
			const FGuid ID = AttacksInProgress[InGiver];
			OnWeaponAttackEnded.Broadcast(ID);
			DamageTaken.Remove(ID);
			AttacksInProgress.Remove(InGiver);
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
	bool IsDamageTakenBy(const ICommonPawn* InPawn, const UObject* InGiver) const
	{
		if (AttacksInProgress.Contains(InGiver))
		{
			return IsDamageTakenBy(InPawn, AttacksInProgress[InGiver]);
		}

		return false;
	}

	FGuid GetCurrentAttackID(const UObject* InGiver) const
	{
		if (AttacksInProgress.Contains(InGiver))
		{
			return AttacksInProgress[InGiver];
		}

		return {};
	}

	void PushItem(const USG_WSItemMetadata* InItem, AActor* InInstigator, AActor* InTarget);

	void PushTicket(TWeakPtr<FEventTicket> Ticket);
	bool IsElementalTracking(const AActor* InTarget) const { return ElementTrackingObjects.Contains(InTarget); }
	FElementTrackingMap& AddElementTracking(AActor* InTarget) { return ElementTrackingObjects.Add(InTarget); }
	FElementTrackingMap& GetElementTracking(const AActor* InTarget) { return ElementTrackingObjects[InTarget]; }
	void RemoveElementTracking(const AActor* InTarget)
	{
		if (ElementTrackingObjects.Contains(InTarget))
		{
			ElementTrackingObjects.Remove(InTarget);
		}
	}

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
