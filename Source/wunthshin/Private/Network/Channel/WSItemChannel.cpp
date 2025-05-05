// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/Channel/WSItemChannel.h"
#include "Network/Channel/WSLoginChannel.h"
#include "Network/Subsystem/WSServerSubsystem.h"

void UWSItemChannel::ReceivedBunch(MessageBase& Bunch)
{
	switch (Bunch.GetType()) 
	{
	case EMessageType::AddItemResponse:
	{
		auto& AddItemMessage = CastTo<EMessageType::AddItemResponse>( Bunch );
		if (AddItemMessage.Success) 
		{
			OnItemAdded.Broadcast((EItemType)AddItemMessage.ItemType, AddItemMessage.itemID, AddItemMessage.Count);
		}
		break;
	}
	case EMessageType::GetItemsResponse:
	{
		auto& GetItemMessage = CastTo<EMessageType::GetItemsResponse>( Bunch );
		if (GetItemMessage.success)
		{
			TArray<FItemAndCountUE> Items;
			for (size_t i = 0; i < GetItemMessage.count; ++i)
			{
				Items.Emplace(GetItemMessage.items[i]);
			}
			RequestItemReceived.Broadcast(GetItemMessage.end, GetItemMessage.section, GetItemMessage.count, Items);
		}
		break;
	}
	default: check(false);
	}
}

void UWSItemChannel::SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch)
{
}
