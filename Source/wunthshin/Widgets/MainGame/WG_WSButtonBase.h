// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wunthshin/Widgets/WG_WSUserWidgetBase.h"
#include "WG_WSButtonBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickButton, int, a);
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSButtonBase : public UWG_WSUserWidgetBase
{
	GENERATED_BODY()

	protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RemainedCoolDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* KeyText;

	
};
