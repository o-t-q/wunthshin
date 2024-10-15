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
	EnablePickUp();
	OnPickUp.AddUniqueDynamic(this, &UC_WSPickUp::HandleOnPickUp);
}

void UC_WSPickUp::HandleOnPickUp(AActor* InTriggeredActor)
{
	if (InTriggeredActor)
	{
		I_WSTaker* TakingObject = Cast<I_WSTaker>(InTriggeredActor);
		ensureAlwaysMsgf(TakingObject, TEXT("Triggered Object is not a taking object!"));

		if (TakingObject->Take(this))
		{
			DisablePickUp();
		}
		else
		{
			UE_LOG(LogPickUpComponent, Log, TEXT("IWS_Taker returns false"));
		}
	}
}

void UC_WSPickUp::EnablePickUp()
{
	if (AActor* OwningActor = GetOwner())
	{
		OwningActor->OnActorBeginOverlap.AddUniqueDynamic(this, &UC_WSPickUp::OnActorBeginOverlapProxy);
	}
}

void UC_WSPickUp::DisablePickUp()
{
	if (AActor* OwningActor = GetOwner())
	{
		OwningActor->OnActorBeginOverlap.RemoveAll(this);
	}
}

void UC_WSPickUp::OnActorBeginOverlapProxy(AActor* OverlappedActor, AActor* /*OtherActor*/)
{
	OnPickUp.Broadcast(OverlappedActor);
}


// Called every frame
void UC_WSPickUp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

