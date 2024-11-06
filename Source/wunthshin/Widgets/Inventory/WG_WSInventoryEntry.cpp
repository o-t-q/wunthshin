

#include "WG_WSInventoryEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"

void UWG_WSInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	Data = Cast<UInventoryEntryData>(ListItemObject);
	if (!Data) return;

	Backgrounds->SetActiveWidgetIndex(int32(Data->EntryData.Metadata->ItemRarity));
	ItemCount->SetText(FText::FromString(FString::FromInt(Data->EntryData.Count)));
	ItemIcon->SetBrushFromTexture(Data->EntryData.Metadata->GetItemIcon());
}
