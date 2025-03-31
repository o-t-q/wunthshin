#include "wunthshin/Network/Channel/Public/WSRegisterChannel.h"

void UWSRegisterChannel::ReceivedBunch(MessageBase& Bunch)
{
	switch ( Bunch.GetType() )
	{
	case EMessageType::RegisterStatus:
	{
		RegisterStatusMessage& registerMessage = reinterpret_cast<RegisterStatusMessage&>(Bunch);
		LastRegistrationStatus.Broadcast( registerMessage.success, (ERegisterFailCodeUE)registerMessage.code );
	}
	default: check(false);
	}
}

void UWSRegisterChannel::SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch)
{
}
