#include "Network/Channel/WSRegisterChannel.h"

void UWSRegisterChannel::ReceivedBunch(MessageBase& Bunch)
{
	switch ( Bunch.GetType() )
	{
	case EMessageType::RegisterStatus:
	{
		RegisterStatusMessage& registerMessage = reinterpret_cast<RegisterStatusMessage&>(Bunch);
		LastRegistrationStatus.Broadcast( registerMessage.messageIdentifier, registerMessage.success, (ERegisterFailCodeUE)registerMessage.code );
		break;
	}
	default: check(false);
	}
}

void UWSRegisterChannel::SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch)
{
}
