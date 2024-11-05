// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WG_WSInGameBundle.generated.h"

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

// public:
// 	UFUNCTION()
// 	void SetActive();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UUserWidget>> ChildWidgets;
};


