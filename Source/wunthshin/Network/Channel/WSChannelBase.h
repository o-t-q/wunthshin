#pragma once

#include "CoreMinimal.h"
#include "Engine/Channel.h"
#include "message.h"

#include "WSChannelBase.generated.h"

UCLASS()
class WUNTHSHIN_API UWSChannelBase : public UChannel
{
	GENERATED_BODY()
public:	
	template <EMessageType MessageType> requires CMessageConstraint<MessageType>
	void SendBunch(FOutBunch& Bunch)
	{
		FOutPacketTraits OutPacketTraits;
		SendBunchInternal(MessageType, Bunch);
		Connection->LowLevelSend((void*)&Bunch, G_MessageSize.at((size_t)MessageType), OutPacketTraits);
	}
	virtual void ReceivedBunch(FInBunch& Bunch) override {};

protected:
	virtual void SendBunchInternal(EMessageType& MessageType, FOutBunch& Bunch) {}
};
