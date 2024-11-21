// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FCTweenInstance.h"
#include "WG_WSUserWidgetBase.generated.h"


class UImage;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	bool InitializeWidget();

public:
	UFUNCTION()
	void OnVisibleWidget();
	UFUNCTION()
	void OnHideWidget();
	
protected:
	void SetVisibleWidget(bool bIsVisible);
	void SetGamePaused(bool bIsGamePaused);
	
protected:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TMap<FName,class UWG_WSUserWidgetBase*> ChildWidgets;
};
