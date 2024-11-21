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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool bInAnimation = false;

protected:
	virtual void NativeConstruct() override;
	bool InitializeWidget();

public:
	UFUNCTION()
	void OnVisibleWidget();
	UFUNCTION()
	void OnHideWidget();

	void SetInAnimation(const bool InValue) { bInAnimation = InValue; }
	bool IsInAnimation() const { return bInAnimation; }
	
protected:
	void SetVisibleWidget(bool bIsVisible);

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TMap<FName,class UWG_WSUserWidgetBase*> ChildWidgets;
};
