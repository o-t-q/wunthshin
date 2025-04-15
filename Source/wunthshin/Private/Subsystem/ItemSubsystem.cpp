// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ItemSubsystem.h"

#include "Data/Item/SG_WSItemMetadata.h"
#include "Data/Effect/EffectRowHandle.h"
#include "Data/Item/ItemTableRow.h"
#include "Data/Item/LootingBoxTableRow.h"
#include "Subsystem/Utility.h"
#include "Subsystem/WeaponSubsystem.h"
#include "Network/Subsystem/WSServerSubsystem.h"
#include "Network/Channel/WSItemChannel.h"

UItemSubsystem::UItemSubsystem()
	: DataTable(nullptr), LootingBoxTable(nullptr)
{
}

void UItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'")));
	check(DataTable);

	LootingBoxTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_LootingBoxTable.DT_LootingBoxTable'")));
	check(LootingBoxTable);
	
	FItemSubsystemUtility::UpdateTable<FItemTableRow>(DataTable, ItemMetadata.MetadataByID, ItemMetadata.Metadata);
	FItemSubsystemUtility::UpdateTable<FLootingBoxTableRow>(LootingBoxTable, LootingBoxMetadata.MetadataByID, LootingBoxMetadata.Metadata);
	DataTableMapping.Emplace(FItemTableRow::StaticStruct(), DataTable);
	DataTableMapping.Emplace(FLootingBoxTableRow::StaticStruct(), LootingBoxTable);

	GOnServerSubsystemInitialized.AddUniqueDynamic(this, &UItemSubsystem::SubscribeServerSubsystemLazy);
}

USG_WSItemMetadata* UItemSubsystem::GetMetadata(const EItemType InItemType, const FName& InAssetName)
{
	switch (InItemType) 
	{
	case EItemType::Consumable:
		return FItemSubsystemUtility::GetMetadataTemplate(ItemMetadata.Metadata, InAssetName);
	case EItemType::LootingBox:
		return FItemSubsystemUtility::GetMetadataTemplate(LootingBoxMetadata.Metadata, InAssetName);
	default: check(false);
	}

	return nullptr;
}

USG_WSItemMetadata* UItemSubsystem::GetMetadata(const EItemType InItemType, const int32 InID)
{
	switch (InItemType)
	{
	case EItemType::Consumable:
		return FItemSubsystemUtility::GetMetadataByIDTemplate(ItemMetadata.MetadataByID, InID);
	case EItemType::LootingBox:
		return FItemSubsystemUtility::GetMetadataByIDTemplate(LootingBoxMetadata.MetadataByID, InID);
	default: check(false);
	}

	return nullptr;
}

FSharedInventory& UItemSubsystem::GetSharedInventory() 
{ 
	return SharedInventory; 
}

void UItemSubsystem::AddItemFromMessage(const EItemType ItemType, const int32 InID, const int32 InCount)
{
	TScriptInterface<IItemMetadataGetter> Interface;
	switch (ItemType)
	{
	case EItemType::Consumable:
		Interface = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>();
		break;
	case EItemType::Weapon:
		Interface = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponSubsystem>();
		break;
	default: check(false);
	}

	check(Interface);
	if (Interface)
	{
		SharedInventory.AddItem(Interface->GetMetadata(ItemType, InID), InCount);
		OnCharacterInventoryUpdated.Broadcast();
	}
}

void UItemSubsystem::UpdateInventory(const bool IsEnd, const int32 Page, const int32 Count, const TArray<FItemAndCountUE>& InItems)
{
	if (Count == 0)
	{
		return;
	}

	UItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>();
	check(ItemSubsystem);
	if (!ItemSubsystem)
	{
		return;
	}

	if (Page == 0)
	{
		ItemSubsystem->GetSharedInventory().Clear(Count);
	}

	for (int i = 0; i < Count; ++i)
	{
		TScriptInterface<IItemMetadataGetter> Interface;
		switch (InItems[i].ItemType)
		{
		case EItemType::Consumable:
			Interface = GetWorld()->GetGameInstance()->GetSubsystem<UItemSubsystem>();
			break;
		case EItemType::Weapon:
			Interface = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponSubsystem>();
			break;
		default: check(false);
		}

		ItemSubsystem->GetSharedInventory().AddItem(Interface->GetMetadata(InItems[i].ItemType, InItems[i].ItemID), InItems[i].Count);
	}

	if (!IsEnd)
	{
		if (UWSServerSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>())
		{
			Subsystem->TryGetItems(Page + 1);
		}
	}
	else
	{
		OnCharacterInventoryFetched.Broadcast();
	}
}

void UItemSubsystem::SubscribeServerSubsystemLazy()
{
	if (UWSServerSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>())
	{
		if (UWSItemChannel* ItemChannel = Subsystem->GetItemChannel())
		{
			ItemChannel->OnItemAdded.AddUniqueDynamic(this, &UItemSubsystem::AddItemFromMessage);
			ItemChannel->RequestItemReceived.AddUniqueDynamic(this, &UItemSubsystem::UpdateInventory);
		}
	}
}
