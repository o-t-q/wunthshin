// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSPickUp.h"

#include "wunthshin/Interfaces/Taker/Taker.h"
#include "wunthshin/Actors/Item/A_WSItem.h"

DEFINE_LOG_CATEGORY(LogPickUpComponent);

// Sets default values for this component's properties
UC_WSPickUp::UC_WSPickUp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UC_WSPickUp::SetTaken(const TScriptInterface<I_WSTaker>& InTaker)
{
	if (InTaker) 
	{
		OnPickUpSuccess.Broadcast(InTaker);
		bTaken = true;
		SetActive(false, false);
	}
	else 
	{
		bTaken = false;
		SetActive(true, false);
	}
}

void UC_WSPickUp::SetActive(bool bNewActive, bool bReset)
{
	Super::SetActive(bNewActive, bReset);

	// 줍기/떨어뜨리기 핸들링을 위한 delegation 리스닝 처리
	if (bNewActive)
	{
		OnPickUp.AddUniqueDynamic(this, &UC_WSPickUp::HandleOnPickUp);
	}
	else
	{
		OnPickUp.RemoveAll(this);
	}
}

// Called when the game starts
void UC_WSPickUp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	bTaken = false;
	OnPickUp.AddUniqueDynamic(this, &UC_WSPickUp::HandleOnPickUp);
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
			// SetTaken을 우회하고 flag만 변경
			// 차후 Taken Flag를 이용하는 상황에 맞추기 위함
			bTaken = true;

			// 주워진 물체를 파괴함
			GetOwner()->Destroy();
		}
		else
		{
			UE_LOG(LogPickUpComponent, Log, TEXT("IWS_Taker::Take returns false"));
		}
	}
}
