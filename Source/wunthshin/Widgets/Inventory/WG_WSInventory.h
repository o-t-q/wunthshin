// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wunthshin/Widgets/WG_WSUserWidgetBase.h"
#include "WG_WSInventoryEntry.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "WG_WSInventory.generated.h"


class UWG_WSInventoryEntry;
class UC_WSInventory;
class UButton;
class UTextBlock;
class UImage;
enum class ERarity : uint8;


/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSInventory : public UWG_WSUserWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UFUNCTION()
	void OnRefreshListItem(ESlateVisibility IsVisibility) { RefreshListItem(); }
	UFUNCTION()
	void OnRefreshListItemChangedItem(UWG_WSInventoryEntry* InObject)
	{
		SelectedEntry = InObject;
		RefreshListItem();
	}
	UFUNCTION()
	void RefreshListItem();

protected:
	UPROPERTY()
	UC_WSInventory* PlayerInventory;

	UPROPERTY()
	UWG_WSInventoryEntry* SelectedEntry;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTileView* TileView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTextBlock* InventoryCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UButton* Button_CloseInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTextBlock* ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTextBlock* ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTextBlock* ItemEfficiency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTextBlock* ItemDescription;

public:
	UFUNCTION()
	void OnClickButton_CloseInventory();
};
