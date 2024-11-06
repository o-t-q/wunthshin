// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSInGameBundle.h"

#include "Components/Button.h"
#include "wunthshin/Widgets/Inventory/WG_WSInventory.h"

void UWG_WSInGameBundle::NativeConstruct()
{
	Super::NativeConstruct();
	
	for (auto& ChildWidget : ChildWidgets)
	{
		ChildWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	//Button_OpenInventory->OnClicked.AddDynamic(this, &ThisClass::SetActive);
}

void UWG_WSInGameBundle::SetActive()
{
	
}
