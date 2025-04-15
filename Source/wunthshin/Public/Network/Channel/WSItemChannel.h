// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Network/Channel/WSChannelBase.h"
#include "WSItemChannel.generated.h"

enum class EItemType : uint8;

USTRUCT()
struct FItemAndCountUE 
{
	GENERATED_BODY()

	UPROPERTY()
	EItemType ItemType;
	UPROPERTY()
	int32 ItemID;
	UPROPERTY()
	int32 Count;

	FItemAndCountUE() : ItemType((EItemType)0), ItemID(0), Count(0) {}

	FItemAndCountUE(const ItemAndCount& Original) : ItemType((EItemType)Original.ItemType), ItemID(Original.ItemID), Count(Original.Count)
	{
	}
};

DEFINE_CHANNEL_MESSAGE(ItemChannel, EMessageChannelType::Item, AddItem, EMessageType::AddItem);
DEFINE_CHANNEL_MESSAGE(ItemChannel, EMessageChannelType::Item, GetItems, EMessageType::GetItemsRequest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemAdded, EItemType, ItemType, int32, ItemID, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRequestItemReceived, bool, IsEnd, int32, Page, int32, Count, const TArray<FItemAndCountUE>&, Items);

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWSItemChannel : public UWSChannelBase
{
	GENERATED_BODY()

public:
	FOnItemAdded OnItemAdded;

	FRequestItemReceived RequestItemReceived;
	
	virtual void ReceivedBunch(MessageBase& Bunch) override;

protected:
	virtual void SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch) override;
};
