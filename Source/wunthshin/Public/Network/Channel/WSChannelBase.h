#pragma once

#include "CoreMinimal.h"
#include "Engine/Channel.h"
#include "message.h"

#include "WSChannelBase.generated.h"

class UWSServerSubsystem;

#define DEFINE_CHANNEL_MESSAGE(ChannelName, _ChannelIndex, Name, Enum) \
class UWSChannelBase; \
class UNetConnection; \
struct FNet##ChannelName##Name##Message \
{ \
	static_assert(CMessageConstraint<Enum>); \
public: \
	static void Send(UNetConnection* Conn, MessageTypeResolver<Enum>::type& Bunch) \
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
	template <typename T, EMessageType MessageType = T::message_type, typename U = MessageT<MessageType>> 
		requires std::is_base_of_v<U, T> && CMessageConstraint<MessageType>
	void SendBunch(T& Bunch)
	{
		FOutPacketTraits OutPacketTraits;
		SendBunchInternal(MessageType, Bunch);
		Connection->LowLevelSend((void*)&Bunch, G_MessageSize.at( (size_t)MessageType ), OutPacketTraits);
	}
	virtual void ReceivedBunch(MessageBase& Bunch) {};

protected:
	virtual void SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch) {}

	// 서브 시스템을 채널에서 필요로 한 경우 사용. 
	// 채널의 패키지가 다르기 때문에 GetWorld()를 통해 동일한 서브시스템을 가져올 수 없음.
	UWSServerSubsystem* GetSubsystem();
};
