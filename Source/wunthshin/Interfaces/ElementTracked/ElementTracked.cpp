// Fill out your copyright notice in the Description page of Project Settings.


#include "ElementTracked.h"

#include "wunthshin/Subsystem/GameInstanceSubsystem/Element/ElementSubsystem.h"


// Add default functionality here for any IMyClass functions that are not pure virtual.
void IElementTracked::ApplyElement(AActor* InInstigator, const FElementRowHandle& InHandle)
{
	// 유효하지 않은 원소
	if (InHandle.Handle.IsNull())
	{
		return;
	}
	
	AActor* Self = Cast<AActor>(this);
	check(Self);
	const UWorld* World = Self->GetWorld();

	if (!World)
	{
		return;
	}

	UElementSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UElementSubsystem>();

	if (!Subsystem)
	{
		return;
	}

	Subsystem->ApplyElement(Self, InInstigator, InHandle);
}
