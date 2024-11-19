// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSInGameBundle.h"

#include "FCTween.h"
#include "FCTweenUObject.h"
#include "wunthshin/Widgets/Inventory/WG_WSInventory.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/NamedSlot.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"

UImage* UWG_WSInGameBundle::FadeImageStatic = nullptr;

void UWG_WSInGameBundle::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeWidget();
	
	FadeImageStatic = FadeImage;

	// 시작할 때 메뉴가 열려있으면 다 닫음
	for(auto& Widget : ChildWidgets)
	{
		Widget.Value->OnHideWidget();
	}
	
	// 메뉴버튼 이벤트 바인딩
	Button_OpenInventory->OnClicked.AddDynamic(this,&ThisClass::OpenWindowInventory);

	// 우측 캐릭터 변경 슬롯 초기화
	InitCharacterSlots();
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
	float Start = bIsIn ? 1.0f : 0.0f;
	float End = 1.f - Start;
	
	return FCTween::Play(Start, End, [&](float t) { FadeImageStatic->SetOpacity(t); }, InDuration);
}

void UWG_WSInGameBundle::OpenWindow(FName InWindowName)
{
	FadeInOut(false)->SetOnComplete([=, this] ()
	{
		ChildWidgets[InWindowName]->OnVisibleWidget();
	})->CreateUObject(this);

	FadeInOut(true);
}

void UWG_WSInGameBundle::InitCharacterSlots()
{
	//auto test = ULocalPlayer::GetSubsystem<UCharacterSubsystem>()->GetRowValue<FCharacterTableRow>(TEXT("YinLin"));
	TArray<UObject*> Objects;
	Objects.Add(NewObject<UObject>(this,FName("YinLin")));
	CharacterRoot->SetListItems(Objects);
}






