// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSPickUp.h"

#include "wunthshin/Interfaces/Taker/Taker.h"

DEFINE_LOG_CATEGORY(LogPickUpComponent);

// Sets default values for this component's properties
UC_WSPickUp::UC_WSPickUp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UC_WSPickUp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	bTaken = false;
	OnPickUp.AddUniqueDynamic(this, &UC_WSPickUp::HandleOnPickUp);
	OnDropping.AddUniqueDynamic(this, &UC_WSPickUp::HandleOnDropping);
}

void UC_WSPickUp::HandleOnPickUp(TScriptInterface<I_WSTaker> InTriggeredActor)
{
	// 이미 해당 물건의 소유자가 있으므로 무시
	if (bTaken)
	{
		// 만약 소유자가 없다면 예외처리
		check(GetOwner());
		UE_LOG(LogPickUpComponent, Log, TEXT("Object is already taken, Owner: %s"), *GetOwner()->GetName());
		return;
	}
	
	if (InTriggeredActor)
	{
		if (InTriggeredActor->Take(this))
		{
			bTaken = true;

			// 물체를 시야에서 숨기고, 충돌 처리를 끔
			GetOwner()->SetActorEnableCollision(false);
			GetOwner()->SetActorHiddenInGame(true);
		}
		else
		{
			UE_LOG(LogPickUpComponent, Log, TEXT("IWS_Taker::Take returns false"));
		}
	}
}

void UC_WSPickUp::HandleOnDropping(TScriptInterface<I_WSTaker> InTriggeringActor)
{
	if (!bTaken)
	{
		UE_LOG(LogPickUpComponent, Log, TEXT("Object is not taken by anyone"));
		return;
	}

	if (InTriggeringActor)
	{
		if (InTriggeringActor->Drop(this))
		{
			bTaken = false;

			// 떨어뜨리는 객체의 위치 근방으로 이동시킴
			if (const AActor* ActorCheck = Cast<AActor>(InTriggeringActor.GetInterface()))
			{
				FVector Origin, Extents;
				GetOwner()->GetActorBounds(true, Origin, Extents);

				const FVector XYGap{Extents.X + 1.f, Extents.Y + 1.f, 0.f};
				GetOwner()->SetActorLocation(ActorCheck->GetActorLocation() + XYGap);
			}

			// 물체를 다시 보여줌
			GetOwner()->SetActorEnableCollision(true);
			GetOwner()->SetActorHiddenInGame(false);
		}
		else
		{
			UE_LOG(LogPickUpComponent, Log, TEXT("IWS_Taker::Drop return false"))
		}
	}
}
