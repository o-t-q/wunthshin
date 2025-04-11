#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widget/WG_WSUserWidgetBase.h"
#include "WG_WSInventoryEntry.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryEntryClicked, UWG_WSInventoryEntry*, InEntry);
//DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInventoryEntryClicked, UWG_WSInventoryEntry*, InEntry);
class UInventoryEntryData;

UCLASS()
class WUNTHSHIN_API UWG_WSInventoryEntry : public UWG_WSUserWidgetBase, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	FOnInventoryEntryClicked OnInventoryEntryClicked;
	static UWG_WSInventoryEntry* Selected;
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

public:
	UInventoryEntryData* GetData() const { return Data; }

protected:
	UPROPERTY()
	UInventoryEntryData* Data;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	class UTextBlock* ItemCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	class UWidgetSwitcher* Backgrounds;
};
