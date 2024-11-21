// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSInGameBundle.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FCTween.h"
#include "FCTweenUObject.h"
#include "wunthshin/Widgets/Inventory/WG_WSInventory.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "Kismet/GameplayStatics.h"

#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"

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

	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		CharacterSubsystem->OnCharacterAdded.AddUniqueDynamic(this, &UWG_WSInGameBundle::InitCharacterSlots);
		
		// 우측 캐릭터 변경 슬롯 초기화
		InitCharacterSlots();
	}
}

void UWG_WSInGameBundle::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (const APlayerController* PlayerController = GetPlayerContext().GetPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
			!Subsystem->HasMappingContext(InputMappingContext))
		{
			Subsystem->AddMappingContext(InputMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetPlayerContext().GetPlayerController()->InputComponent))
		{
			EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &UWG_WSInGameBundle::OpenWindowInventory);
		}
	}
}

void UWG_WSInGameBundle::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (const APlayerController* PlayerController = GetPlayerContext().GetPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
			Subsystem->HasMappingContext(InputMappingContext))
		{
			Subsystem->RemoveMappingContext(InputMappingContext);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetPlayerContext().GetPlayerController()->InputComponent))
		{
			EnhancedInputComponent->ClearBindingsForObject(this);
		}
	}
}

void UWG_WSInGameBundle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

UWG_WSInGameBundle::UWG_WSInGameBundle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  FadeImage(nullptr),
	  Button_OpenInventory(nullptr),
	  CharacterRoot(nullptr)
{
	if (static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_HUD(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ThirdPerson/Input/IMC_HUD.IMC_HUD'"));
		IMC_HUD.Succeeded())
	{
		InputMappingContext = IMC_HUD.Object;
	}

	if (static ConstructorHelpers::FObjectFinder<UInputAction> IA_Inventory(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Inventory.IA_Inventory'"));
		IA_Inventory.Succeeded())
	{
		InventoryAction = IA_Inventory.Object;
	}
}

FCTweenInstance* UWG_WSInGameBundle::FadeInOut(bool bIsIn, float InDuration)
{
	float Start = bIsIn ? 1.0f : 0.0f;
	float End = 1.f - Start;
	
	return FCTween::Play(Start, End, [&](float t) { FadeImageStatic->SetOpacity(t); }, InDuration);
}

void UWG_WSInGameBundle::OpenWindow(FName InWindowName)
{
	if (ChildWidgets[InWindowName]->GetVisibility() == ESlateVisibility::Hidden)
	{
		FadeInOut(false)->SetOnComplete([this, InWindowName] ()
		{
			ChildWidgets[InWindowName]->OnVisibleWidget();
		})->CreateUObject(this);

		FadeInOut(true);
	}
	else if (ChildWidgets[InWindowName]->GetVisibility() == ESlateVisibility::Visible)
	{
		FadeInOut(false, 0.5f)->SetOnComplete([this, InWindowName]()
		{
			ChildWidgets[InWindowName]->OnHideWidget();
		});

		FadeInOut(true, 0.5f);
	}
	
}

void UWG_WSInGameBundle::InitCharacterSlots()
{
	CharacterRoot->ClearListItems();
	
	// 1. 현재 레벨 상 캐릭터가 파괴되지 않고 유지되는 상태이므로 캐릭터 자체를 데이터로 사용함
	// 2. HUD도 레벨 변경에 맞춰 동시에 파괴되기 때문에 캐릭터가 파괴되기 전까지는 동일한 포인터를 유지하고 있게됨
	if (const UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		CharacterRoot->SetListItems(CharacterSubsystem->GetOwnedCharacters());
	}
}






