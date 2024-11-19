// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSCharacterChangerEntry.h"

#include "InputMappingContext.h"
#include "Components/Image.h"
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
		KeyText->SetText(FText::FromString(Row->CharacterName.ToString()));
		CharacterIcon->SetBrushFromTexture(Row->CharacterIcon);

		// 캐릭터의 체력변화 추적
		CurrentHP->PercentDelegate.BindDynamic(Character->GetStatsComponent(), &UStatsComponent::GetHPRatioNonConst);
		CurrentHP->SynchronizeProperties();
	}

	BindKey();
}

void UWG_WSCharacterChangerEntry::BindKey()
{
	// 현재 맵핑된 키
	auto Player = Cast<AA_WSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	auto ActionMapping = Player->GetMappingContext()->GetMappings();
	if (!ActionMapping.IsEmpty())
	{
		for (auto& Mapping : ActionMapping)
		{
			FString ActionName = Mapping.Action.GetName();
			if (ActionName.Contains("IA_Character"))
				KeyText->SetText(FText::FromString(ActionName));
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
