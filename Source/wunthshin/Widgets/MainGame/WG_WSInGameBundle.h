// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "wunthshin/Widgets/WG_WSUserWidgetBase.h"
#include "WG_WSInGameBundle.generated.h"

class UListView;
class UVerticalBox;
class FCTween;
class UImage;
class UButton;
class UWG_WSInventory;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSInGameBundle : public UWG_WSUserWidgetBase
{
	GENERATED_BODY()

	enum class EChildWidget
	{
		Inventory,
		Status,
		Menu,
	};

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	static FCTweenInstance* FadeInOut(bool bIsIn = false, float InDuration = 1.f);

protected:
	void OpenWindow(FName InWindowName);

	UFUNCTION()
	void OpenWindowInventory() { OpenWindow("Window_Inventory"); };

protected:
	void InitCharacterSlots();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* FadeImage;
	static UImage* FadeImageStatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_OpenInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UListView* CharacterRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_NormalAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Skill01;
};
