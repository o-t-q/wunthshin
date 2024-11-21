// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wunthshin/Widgets/WG_WSUserWidgetBase.h"
#include "WG_WSInventoryEntry.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "wunthshin/Enums.h"
#include "WG_WSInventory.generated.h"


class IInventoryComponent;
class UCheckBox;
enum class EItemType : uint8;
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

	UFUNCTION()
	void OnToggleCategory_Consumable(bool bIsActive) { ChangeCategory(EItemType::Consumable); }

	UFUNCTION()
	void OnToggleCategory_Weapon(bool bIsActive) { ChangeCategory(EItemType::Weapon); }

	UFUNCTION()
	void ChangeCategory(EItemType InItemType);

protected:
	
	IInventoryComponent* PlayerInventory;

	UPROPERTY()
	UWG_WSInventoryEntry* SelectedEntry;

	UPROPERTY(meta = (Bitmask, BitmaskEnum = EItemType))
	EItemType CurrentCategory;

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

	UPROPERTY()
	UUserWidget* ToggleGroup_Category;

	UPROPERTY()
	TArray<UCheckBox*> CheckBoxes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InventoryCategory", meta=(BindWidget))
	UCheckBox* Toggle_CategoryWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InventoryCategory", meta=(BindWidget))
	UCheckBox* Toggle_CategoryConsumable;

public:
	UFUNCTION()
	void OnClickButton_CloseInventory();
};
