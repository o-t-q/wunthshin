// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WG_WSInGameBundle.generated.h"

class UWG_WSInventory;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickMenuButton, int, test);

class UButton;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSInGameBundle : public UUserWidget
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
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TArray<class UWG_WSUserWidgetBase*> ChildWidgets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWG_WSInventory* Window_Inventory;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UWG_WSUserWidgetBase* Window_Inventory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_OpenInventory;
};


