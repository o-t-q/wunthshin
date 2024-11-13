// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"
#include "WG_WSCharacterChangerEntry.generated.h"

struct FCharacterTableRow;
class UTextBlock;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClick);

/******************************************************
 *
 *	@Feature
 *	- 클릭 또는 바인딩 된 키 입력 시 플레이어 캐릭터 변경
 *
 */
UCLASS()
class WUNTHSHIN_API UWG_WSCharacterChangerEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

public:
	// 캐릭터 변경시 호출
	UFUNCTION()
	void RefreshEntry();

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY()
	FCharacterTableRow Data;

protected:
	// UPROPERTY(meta = (BindWidget))
	// UMaterial* CharacterIcon;
	
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KeyText;

	// UPROPERTY(meta = (BindWidget))
	// UProgressBar* CurrentHP;

protected:
	FOnClick OnClick;
};
