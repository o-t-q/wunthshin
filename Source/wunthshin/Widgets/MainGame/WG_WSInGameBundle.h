// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "wunthshin/Widgets/WG_WSUserWidgetBase.h"
#include "WG_WSInGameBundle.generated.h"

class FCTween;
class FCTweenInstanceFloat;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickMenuButton, int, test);

class UImage;
class UButton;
class FCTweenInstance;
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
	FCTweenInstance* FadeInOut(bool bIsIn,float InDuration = 1.f);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* FadeImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_OpenInventory;
};
