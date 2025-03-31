#pragma once

#include "CoreMinimal.h"
#include "Engine/Channel.h"
#include "message.h"

#include "WSChannelBase.generated.h"

#define DEFINE_CHANNEL_MESSAGE(ChannelName, _ChannelIndex, Name, Enum) \
class UWSChannelBase; \
class UNetConnection; \
struct FNet##ChannelName##Name##Message \
{ \
	static_assert(CMessageConstraint<Enum>); \
public: \
	/** 지정된 연결의 제어 채널로 이 유형의 메시지를 보냅니다. \
	* @note: const not used only because of the FArchive interface; the parameters are not modified \
	*/ \
	template <typename NewBunch> requires std::is_base_of_v<MessageBase, NewBunch> \
	static void Send(UNetConnection* Conn, NewBunch& Bunch) \
	{ \
		if (Conn->Channels.IsValidIndex((int32_t)_ChannelIndex) && Conn->Channels[(int32_t)_ChannelIndex] != NULL) \
		{ \
			Cast<UWSChannelBase>(Conn->Channels[(int32_t)_ChannelIndex])->SendBunch(Bunch); \
		} \
	} \
	/** receives a message of this type from the passed in bunch */ \
	template <typename NewBunch> requires std::is_base_of_v<MessageBase, NewBunch> \
	[[nodiscard]] static bool Receive(NewBunch& Bunch) \
	{ \
		return Bunch.GetType() == Enum; \
	} \
};

UCLASS()
class WUNTHSHIN_API UWSChannelBase : public UChannel
{
	GENERATED_BODY()
public:	
	template <EMessageType MessageType> requires CMessageConstraint<MessageType>
	void SendBunch(MessageT<MessageType>& Bunch)
	{
		FOutPacketTraits OutPacketTraits;
		SendBunchInternal(MessageType, Bunch);
		Connection->LowLevelSend((void*)&Bunch, G_MessageSize.at((size_t)MessageType), OutPacketTraits);
	}
	virtual void ReceivedBunch(MessageBase& Bunch) {};

protected:
	virtual void SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch) {}
};
