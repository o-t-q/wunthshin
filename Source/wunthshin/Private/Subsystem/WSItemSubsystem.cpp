// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/WSItemSubsystem.h"

#include "Actor/Pawn/AA_WSCharacter.h"
#include "Component/C_WSCharacterInventory.h"
#include "Controller/AwunthshinSpawnPlayerController.h"
#include "Controller/wunthshinPlayerController.h"
#include "Data/Item/SG_WSItemMetadata.h"
#include "Data/Effect/EffectRowHandle.h"
#include "Data/Item/ItemTableRow.h"
#include "Data/Item/LootingBoxTableRow.h"
#include "Data/Item/WSSharedInventory.h"
#include "Interface/InventoryComponent.h"
#include "Subsystem/Utility.h"
#include "Subsystem/WeaponSubsystem.h"
#include "Network/Subsystem/WSServerSubsystem.h"
#include "Network/Channel/WSItemChannel.h"

UWSItemSubsystem::UWSItemSubsystem()
	: DataTable(nullptr), LootingBoxTable(nullptr)
{
}

void UWSItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
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

	GOnServerSubsystemInitialized.AddUniqueDynamic(this, &UWSItemSubsystem::SubscribeServerSubsystemLazy);
}

USG_WSItemMetadata* UWSItemSubsystem::GetMetadata(const EItemType InItemType, const FName& InAssetName)
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

USG_WSItemMetadata* UWSItemSubsystem::GetMetadata(const EItemType InItemType, const int32 InID)
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

void UWSItemSubsystem::AddItemFromMessage(const FUUIDWrapper& InSessionID, const EItemType ItemType, const int32 InID, const int32 InCount)
{
	TScriptInterface<IItemMetadataGetter> Interface;
	switch (ItemType)
	{
	case EItemType::Consumable:
		Interface = GetWorld()->GetGameInstance()->GetSubsystem<UWSItemSubsystem>();
		break;
	case EItemType::Weapon:
		Interface = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponSubsystem>();
		break;
	default: check(false);
	}

	check(Interface);
	if (Interface)
	{
		UWSServerSubsystem* ServerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
		check(ServerSubsystem);
		if (!ServerSubsystem)
		{
			return;
		}
		
		const AwunthshinSpawnPlayerController* PlayerController = Cast<AwunthshinSpawnPlayerController>( ServerSubsystem->GetPlayerController( InSessionID ) );
		check(PlayerController);
		if (!PlayerController)
		{
			return;
		}
		
		PlayerController->GetSharedInventory()->AddItem(Interface->GetMetadata(ItemType, InID), InCount);
		OnCharacterInventoryUpdated.Broadcast();
	}
}

void UWSItemSubsystem::UpdateInventory(const FUUIDWrapper& InSessionID, const bool IsEnd, const int32 Page, const int32 Count, const TArray<FItemAndCountUE>& InItems)
{
	if (Count == 0)
	{
		return;
	}

	UWSItemSubsystem* ItemSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSItemSubsystem>();
	check(ItemSubsystem);
	if (!ItemSubsystem)
	{
		return;
	}

	UWSServerSubsystem* ServerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
	check(ServerSubsystem);
	if (!ServerSubsystem)
	{
		return;
	}

	const AwunthshinSpawnPlayerController* PlayerController = Cast<AwunthshinSpawnPlayerController>( ServerSubsystem->GetPlayerController( InSessionID ) );
	check(PlayerController);
	if (!PlayerController)
	{
		return;
	}

	if (Page == 0)
	{
		PlayerController->GetSharedInventory()->Clear( 1024 );
	}

	for (int i = 0; i < Count; ++i)
	{
		TScriptInterface<IItemMetadataGetter> Interface;
		switch (InItems[i].ItemType)
		{
		case EItemType::Consumable:
			Interface = GetWorld()->GetGameInstance()->GetSubsystem<UWSItemSubsystem>();
			break;
		case EItemType::Weapon:
			Interface = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponSubsystem>();
			break;
		default: check(false);
		}

		PlayerController->GetSharedInventory()->AddItem(Interface->GetMetadata(InItems[i].ItemType, InItems[i].ItemID), InItems[i].Count);
	}

	if (!IsEnd)
	{
		if (UWSServerSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>())
		{
			const bool Result = Subsystem->Server_GetItems( PlayerController, Page + 1 );
			check(Result);
		}
	}
	else
	{
		OnCharacterInventoryFetched.Broadcast();
	}
}

void UWSItemSubsystem::SubscribeServerSubsystemLazy()
{
	if (UWSServerSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>())
	{
		if (UWSItemChannel* ItemChannel = Subsystem->GetItemChannel())
		{
			ItemChannel->OnItemAdded.AddUniqueDynamic(this, &UWSItemSubsystem::AddItemFromMessage);
			ItemChannel->RequestItemReceived.AddUniqueDynamic(this, &UWSItemSubsystem::UpdateInventory);
		}
	}
}
