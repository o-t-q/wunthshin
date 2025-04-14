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
		OnItemAdded.Broadcast( AddItemMessage.itemID );
		break;
	}
	case EMessageType::AllItemResponse:
	{
		auto& GetItemMessage = CastTo<EMessageType::AllItemResponse>( Bunch );
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
	switch (MessageType)
	{
	case EMessageType::AddItem:
	{
		auto& AddItemMessage = CastTo<EMessageType::AddItem>(Bunch);
		UWSServerSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
		AddItemMessage.sessionId = Subsystem->GetSessionID().uuid;
		break;
	}
	case EMessageType::GetItemsRequest:
	{
		auto& GetItemsMessage = CastTo<EMessageType::GetItemsRequest>( Bunch );
		UWSServerSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
		GetItemsMessage.sessionId = Subsystem->GetSessionID().uuid;
		break;
	}
	default: check(false);
	}
}
