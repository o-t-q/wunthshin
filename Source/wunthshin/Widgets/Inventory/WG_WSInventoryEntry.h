#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "wunthshin/Widgets/WG_WSUserWidgetBase.h"
#include "WG_WSInventoryEntry.generated.h"


class UInventoryEntryData;

UCLASS()
class WUNTHSHIN_API UWG_WSInventoryEntry : public UWG_WSUserWidgetBase, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	static UWG_WSInventoryEntry* Selected;
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

public:
	UInventoryEntryData* GetData() const { return Data; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OnClickButton();
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory", meta=(BindWidget))
	class UButton* Button_SelectItem;
};
