// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSCharacterChangerEntry.h"

#include "InputMappingContext.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/PanelWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"

void UWG_WSCharacterChangerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (const AA_WSCharacter* Character = Cast<AA_WSCharacter>(ListItemObject))
	{
		// 약한 참조
		WeakCharacterPtr = Character;

		// 캐릭터의 설명 복사
		const FCharacterTableRow* Row = Character->GetDataTableHandle().GetRow<FCharacterTableRow>(TEXT(""));
		CharacterIcon->SetBrushFromTexture(Row->CharacterIcon);

		// 캐릭터의 체력변화 추적
		CurrentHP->PercentDelegate.BindDynamic(Character->GetStatsComponent(), &UStatsComponent::GetHPRatioNonConst);
		CurrentHP->SynchronizeProperties();

		if (const UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
		{
			CharacterIndex = CharacterSubsystem->GetIndexOfCharacter(Character);
		}

		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			TScriptDelegate<> Delegate;
			Delegate.BindUFunction(this, "DisplayBindKey");
			if (!PlayerController->OnPossessedPawnChanged.Contains(Delegate))
			{
				PlayerController->OnPossessedPawnChanged.Add(Delegate);	
			}
			
			DisplayBindKey();
		}
	}
}

void UWG_WSCharacterChangerEntry::DisplayBindKey() const
{
	// 현재 캐릭터라면 키 바인드 안내 표시를 숨김
	if (const UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
	{
		if (CharacterSubsystem->GetCurrentCharacter() == WeakCharacterPtr)
		{
			KeyOverlay->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
	}
	
	// 그렇지 않다면 현재 맵핑된 키를 보여줌
	KeyOverlay->SetVisibility(ESlateVisibility::Visible);
	
	if (WeakCharacterPtr.IsValid())
	{
		const AA_WSCharacter* Character = WeakCharacterPtr.Get();
		auto ActionMapping = Character->GetMappingContext()->GetMappings();
		if (!ActionMapping.IsEmpty())
		{
			FString CurrentKeyMapping = "IA_Character" + FString::FromInt(CharacterIndex + 1);
			const FEnhancedActionKeyMapping* It = ActionMapping.FindByPredicate([&CurrentKeyMapping](const FEnhancedActionKeyMapping& InMapping)
			{
				return InMapping.Action.GetName() == CurrentKeyMapping;
			});

			if (It)
			{
				KeyText->SetText(It->Key.GetDisplayName(false));
			}
		}	
	}
}

FReply UWG_WSCharacterChangerEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply ReturnValue = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	OnClick.Broadcast();
	
	if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>();
		CharacterSubsystem && WeakCharacterPtr.IsValid())
	{
		CharacterSubsystem->SpawnAsCharacter(CharacterSubsystem->GetIndexOfCharacter(WeakCharacterPtr.Get()));
	}
	
	return ReturnValue;
}
