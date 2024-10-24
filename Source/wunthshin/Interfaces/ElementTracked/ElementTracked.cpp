// Fill out your copyright notice in the Description page of Project Settings.


#include "ElementTracked.h"

#include "wunthshin/Subsystem/ElementSubsystem/ElementSubsystem.h"


// Add default functionality here for any IMyClass functions that are not pure virtual.
void IElementTracked::ApplyElement(AActor* InThis, AActor* InInstigator, const FElementRowHandle& InHandle)
{
	const UWorld* World = InThis->GetWorld();

	if (!World)
	{
		return;
	}

	UElementSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UElementSubsystem>();

	if (!Subsystem)
	{
		return;
	}

	Subsystem->ApplyElement(InThis, InInstigator, InHandle);
}
