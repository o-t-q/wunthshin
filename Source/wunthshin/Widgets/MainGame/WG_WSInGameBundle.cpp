// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSInGameBundle.h"

#include "FCTween.h"
#include "wunthshin/Widgets/Inventory/WG_WSInventory.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"


void UWG_WSInGameBundle::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeWidget();
	UCanvasPanelSlot* image = NewObject<UCanvasPanelSlot>();
	
	auto inven = Cast<UWG_WSInventory>(ChildWidgets["Window_Inventory"]);
	Button_OpenInventory->OnClicked.AddDynamic(inven, &UWG_WSInventory::OnHideWidget);
}

void UWG_WSInGameBundle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UWG_WSInGameBundle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FCTweenInstance* UWG_WSInGameBundle::FadeInOut(bool bIsIn, float InDuration)
{
	const FLinearColor Original = FadeImage->GetColorAndOpacity();
	FadeImage->SetColorAndOpacity(FLinearColor::Black);
	float Start = bIsIn ? 1.0f : 0.0f;
	float End = 1.f - Start;

	return FCTween::Play(Start, End, [&](float t) { FadeImage->SetOpacity(t); }, InDuration);
}


