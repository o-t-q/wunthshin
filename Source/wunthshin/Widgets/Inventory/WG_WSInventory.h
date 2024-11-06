// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickListItem,TScriptInterface<IUserObjectListEntry>, ListItem);

UCLASS()
class WUNTHSHIN_API UCustomListView : public UListView
{
	GENERATED_BODY()
	virtual void OnItemClickedInternal(UObject* Item) override
	{
		auto selected = GetSelectedItem<UWG_WSInventoryEntry>();
		auto data = selected->GetData();

		
	}
};

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSInventory : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	void RefreshListItem();
public:
	void SetDescription(FText InText) { ItemDescription->SetText(InText);}
	void SetItemName(FText InText)	{ItemName->SetText(InText);}
	void SetItemIcon(UTexture2D* InTexture) {ItemIcon->SetBrushFromTexture(InTexture);}
	void SetItemEfficiency(FText InText) {ItemEfficiency->SetText(InText);}
	
protected:
	UPROPERTY()
	UC_WSInventory* PlayerInventory;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTileView* TileView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UTextBlock* InventoryCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	UButton* Button_ClosePanel;

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
	void OnClickButton_ClosePanel();
};
