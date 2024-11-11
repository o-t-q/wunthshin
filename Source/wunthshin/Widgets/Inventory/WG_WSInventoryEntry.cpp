

#include "WG_WSInventoryEntry.h"

#include "WG_WSInventory.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"


UWG_WSInventoryEntry* UWG_WSInventoryEntry::Selected = nullptr;

void UWG_WSInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	Data = Cast<UInventoryEntryData>(ListItemObject);
	if (!Data) return;

	Backgrounds->SetActiveWidgetIndex(int32(Data->EntryData.Metadata->ItemRarity));
	ItemCount->SetText(FText::FromString(FString::FromInt(Data->EntryData.Count)));
	ItemIcon->SetBrushFromTexture(Data->EntryData.Metadata->GetItemIcon());

	// Inventory 부모 클래스가 해당 엔트리의 OnInventoryEntryClicked를 Listen
	auto Parent = Data->Owner;
	OnInventoryEntryClicked.AddUniqueDynamic(Cast<UWG_WSInventory>(Parent), &UWG_WSInventory::OnRefreshListItemChangedItem);
	
	//Cast<UTileView>(GetOwningListView())->OnItemClicked().AddUFunction(this,FName("OnClickButton"));
}

FReply UWG_WSInventoryEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Value = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	OnInventoryEntryClicked.Broadcast(this);	
	return Value;
}

void UWG_WSInventoryEntry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
}

