

#include "WG_WSInventoryEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"

void UWG_WSInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UInventoryEntryData* Data = Cast<UInventoryEntryData>(ListItemObject);
	if (!Data) return;

	RarityBackground->SetBrushFromTexture(Data->RarityBackground);
	ItemCount->SetText(FText::FromString(FString::FromInt(Data->EntryData.Count)));
	ItemIcon->SetBrushFromTexture(Data->EntryData.Metadata->GetItemIcon());
	
}
