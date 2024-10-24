// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wunthshin/Subsystem/TableQuerySubsystem.h"
#include "ElementSubsystem.generated.h"

class UO_WSElementReactor;

DECLARE_LOG_CATEGORY_EXTERN(LogElementSubsystem, Log, All);

USTRUCT()
struct FElementReactionPair 
{
	GENERATED_BODY()

	FDataTableRowHandle Elements[2];

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> Instigators;
};

USTRUCT()
struct FElementTrackingContext 
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	AActor* Instigator;

	FTimerHandle ExpiredHandle;
};

uint32 GetTypeHash(const FDataTableRowHandle& InDataTableHandle);

USTRUCT()
struct FElementTrackingMap
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TMap<FDataTableRowHandle, FElementTrackingContext> Elements;

public:
	void Add(UWorld* InWorldContext, AActor* InInstigator, const FDataTableRowHandle& InElementRow, const float InExpired = 1.f) 
	{
		if (!IsValid(InWorldContext))
		{
			return;
		}

		// 중복된 원소 효과는 무시
		if (Elements.Contains(InElementRow))
		{
			return;
		}

		Elements.Add(InElementRow);

		FTimerDelegate OnExpiredDelegate;
		OnExpiredDelegate.BindRaw(this, &FElementTrackingMap::Remove, InWorldContext, InElementRow, true);

		FTimerManagerTimerParameters TimerParam;
		TimerParam.bLoop = false;
		TimerParam.bMaxOncePerFrame = false;
		TimerParam.FirstDelay = -1.f;

		InWorldContext->GetTimerManager().SetTimer
		(
			Elements[InElementRow].ExpiredHandle,
			OnExpiredDelegate,
			InExpired,
			TimerParam
		);

		Elements[InElementRow].Instigator = InInstigator;
	}

	void Remove(UWorld* InWorldContext, const FDataTableRowHandle InElementRow, const bool bTimer = false) 
	{
		if (!IsValid(InWorldContext)) 
		{
			// 시간이 지나고 레벨이 유효하지 않을때 제거
			// 서버가 없고, 한 상황은 한 레벨에만 유효한 것으로 가정
			if (bTimer && Elements.Contains(InElementRow)) 
			{
				Elements.Remove(InElementRow);
			}

			return;
		}

		if (!Elements.Contains(InElementRow)) 
		{
			return;
		}

		// 시간에 의해 초기화되는 것이 아니라면 타이머 중단
		if (FTimerHandle& TimerHandle = Elements[InElementRow].ExpiredHandle;
			TimerHandle.IsValid()) 
		{
			InWorldContext->GetTimerManager().ClearTimer(TimerHandle);
		}

		Elements.Remove(InElementRow);
	}

	bool IsFull() const 
	{
		return Elements.Num() >= 2;
	}

	FElementReactionPair ExtractElement() 
	{
		if (!IsFull()) 
		{
			return {};
		}

		FElementReactionPair OutPair;
		char Count = 0;

		// 맵의 순차접근을 통해 pair에 부여
		// todo: 들어온 순서대로가 보장되지 않을 수 있음
		for (auto It = Elements.begin(); It; ++It) 
		{
			if (Count == 2)
			{
				break;
			}

			OutPair.Elements[Count] = It->Key;
			OutPair.Instigators[Count] = It->Value.Instigator;
			Count++;
		}

		// 지금까지 부여된 원소 초기화
		Elements.Empty();
		return OutPair;
	}
};

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UElementSubsystem : public UTableQueryGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Reactor", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<UO_WSElementReactor>, UO_WSElementReactor*> PreinstantiatedReactors;

	UPROPERTY(VisibleAnywhere, Category = "Stack", meta = (AllowPrivateAccess = "true"))
	TMap<AActor*, FElementTrackingMap> TrackingObjects;

public:
	UElementSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ApplyElement(AActor* InTarget, AActor* InInstigator, const FDataTableRowHandle& InElementRow);

	UO_WSElementReactor* GetReactor(const FDataTableRowHandle& InLeft, const FDataTableRowHandle& InRight);

	UO_WSElementReactor* GetReactor(TSubclassOf<UO_WSElementReactor> InReactorType) const;
};
