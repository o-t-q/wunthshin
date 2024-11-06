#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "WG_WSInventoryEntry.generated.h"



class UInventoryEntryData;

UCLASS()
class WUNTHSHIN_API UWG_WSInventoryEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
public:
	UInventoryEntryData* GetData() { return Data; }	
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

public:
	UPROPERTY()
	FOnItemClicked OnItemClicked;
};
