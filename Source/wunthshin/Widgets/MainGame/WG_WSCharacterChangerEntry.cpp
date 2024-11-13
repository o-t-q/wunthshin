// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSCharacterChangerEntry.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"

void UWG_WSCharacterChangerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// 대기 중인 캐릭터 정보를 받아옴
	auto AssetName = ListItemObject->GetFName();
	auto Subsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>();
	Data = *Subsystem->GetTableRow(AssetName);
	if (Data.CharacterName.IsNone()) return;

	RefreshEntry();
}

void UWG_WSCharacterChangerEntry::RefreshEntry()
{
	// 이벤트 발동 시 출격할 캐릭터 초상화
	//CharacterIcon->SetBrushFromTexture(Data.CharacterIcon);
	auto Mat = CharacterIcon->GetDynamicMaterial();
	Mat->SetTextureParameterValue(FName("Icon"),Data.CharacterIcon);
	CharacterIcon->SetBrushFromMaterial(Mat);

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

	// 캐릭터 변경 함수 바인딩
	//OnClick.AddUniqueDynamic()
	OnClick.AddUniqueDynamic(this,&ThisClass::RefreshEntry);
	// todo: 현재 체력 출력
}

FReply UWG_WSCharacterChangerEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnClick.Broadcast();
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
