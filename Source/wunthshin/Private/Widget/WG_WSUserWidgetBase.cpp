// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WG_WSUserWidgetBase.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

//UImage* UWG_WSUserWidgetBase::FadeImage = nullptr;

void UWG_WSUserWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	// FadeImage = NewObject<UImage>();
	// FadeImage->SetColorAndOpacity(FLinearColor::Black);
	// FadeImage->AddToRoot()
}

bool UWG_WSUserWidgetBase::InitializeWidget()
{
	// 자식 위젯 수집이 필요한 경우에만 호출
	if(nullptr == WidgetTree) return false;
	TArray<UWidget*> NewArray;
	WidgetTree->GetAllWidgets(NewArray);

	for(auto widget : NewArray)
	{
		if(widget->IsA(UWG_WSUserWidgetBase::StaticClass()))
			ChildWidgets.Emplace(widget->GetFName(),Cast<UWG_WSUserWidgetBase>(widget));
	}
	
	return ChildWidgets.IsEmpty();
}

void UWG_WSUserWidgetBase::SetVisibleWidget(bool bIsVisible)
{
	SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UWG_WSUserWidgetBase::OnVisibleWidget()
{
	SetVisibleWidget(true);
	FInputModeUIOnly UIOnly;
	UIOnly.SetWidgetToFocus(TakeWidget());
	UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	GetPlayerContext().GetPlayerController()->SetInputMode(FInputModeUIOnly{});
	GetPlayerContext().GetPlayerController()->SetShowMouseCursor(true);
}

void UWG_WSUserWidgetBase::OnHideWidget()
{
	SetVisibleWidget(false);
	GetPlayerContext().GetPlayerController()->SetInputMode(FInputModeGameOnly{});
	GetPlayerContext().GetPlayerController()->SetShowMouseCursor(false);
}
