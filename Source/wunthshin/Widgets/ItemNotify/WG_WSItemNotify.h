// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "wunthshin/Widgets/WG_WSUserWidgetBase.h"
#include "WG_WSItemNotify.generated.h"

class AA_WSItem;
class UTextBlock;

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSItemNotify : public UWG_WSUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AA_WSItem* ParentItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* ItemNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* KeyText;

public:
	UFUNCTION(BlueprintCallable)
	void SetParentItem(AA_WSItem* InParent);

	UFUNCTION(BlueprintImplementableEvent)
	void FetchFromParentItem(const AA_WSItem* InParent);

};
