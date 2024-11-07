

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

	Button_SelectItem->OnClicked.AddDynamic(this,&ThisClass::OnClickButton);
	
	Data = Cast<UInventoryEntryData>(ListItemObject);
	if (!Data) return;

	Backgrounds->SetActiveWidgetIndex(int32(Data->EntryData.Metadata->ItemRarity));
	ItemCount->SetText(FText::FromString(FString::FromInt(Data->EntryData.Count)));
	ItemIcon->SetBrushFromTexture(Data->EntryData.Metadata->GetItemIcon());

	//Cast<UTileView>(GetOwningListView())->OnItemClicked().AddUFunction(this,FName("OnClickButton"));
}

void UWG_WSInventoryEntry::OnClickButton()
{
	// auto UI = Cast<UWG_WSInventory>(GetOwningListView()->GetOuter()->GetOuter());
	// UI->RefreshListItem();
	Selected = this;
}