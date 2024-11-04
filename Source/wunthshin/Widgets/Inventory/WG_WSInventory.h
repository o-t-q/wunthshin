// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TileView.h"
#include "WG_WSInventory.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSInventory : public UUserWidget
{
	GENERATED_BODY()

	protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTileView* TileView;
};
