#pragma once

#include "CoreMinimal.h"
#include "Engine/Channel.h"
#include "message.h"

#include "WSChannelBase.generated.h"

#define DEFINE_CHANNEL_MESSAGE(ChannelName, _ChannelIndex, Name, Enum) \
class UARChannel; \
template<> struct FNet##ChannelName##Message<Enum> requires CMessageConstraint<Enum> \
{ \
public: \
	/** 지정된 연결의 제어 채널로 이 유형의 메시지를 보냅니다. \
	* @note: const not used only because of the FArchive interface; the parameters are not modified \
	*/ \
	template <typename NewBunch> requires std::is_base_of_v<MessageBase, NewBunch> \
	static void Send(UNetConnection* Conn, NewBunch& Bunch) \
	{ \
		if (Conn->Channels[_ChannelIndex] != NULL) \
		{ \
			Cast<UARChannel>(Conn->Channels[_ChannelIndex])->SendBunch(Bunch); \
		} \
	} \
	/** receives a message of this type from the passed in bunch */ \
	template <typename NewBunch> requires std::is_base_of_v<MessageBase, NewBunch> \
	[[nodiscard]] static bool Receive(NewBunch& Bunch) \
	{ \
		return Bunch.messageType == Enum; \
	} \
};

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
