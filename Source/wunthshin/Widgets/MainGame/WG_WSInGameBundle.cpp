// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSInGameBundle.h"

#include "AsyncTreeDifferences.h"
#include"Kismet/KismetMathLibrary.h"
#include "Components/Button.h"
#include "wunthshin/Widgets/Inventory/WG_WSInventory.h"

void UWG_WSInGameBundle::NativeConstruct()
{
	Super::NativeConstruct();
	Window_Inventory->SetVisibility(ESlateVisibility::Hidden);
}

void UWG_WSInGameBundle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
