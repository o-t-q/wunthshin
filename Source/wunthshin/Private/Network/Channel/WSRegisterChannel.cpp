#include "Network/Channel/WSRegisterChannel.h"

#include "EntitySystem/MovieSceneEntitySystemRunner.h"

void UWSRegisterChannel::ReceivedBunch(MessageBase& Bunch)
{
	switch ( Bunch.GetType() )
	{
	case EMessageType::RegisterStatus:
	{
		RegisterStatusMessage& registerMessage = reinterpret_cast<RegisterStatusMessage&>(Bunch);
		LastRegistrationStatus.Broadcast( registerMessage.success, (ERegisterFailCodeUE)registerMessage.code );
		break;
	}
	default: check(false);
	}
}

void UWSRegisterChannel::SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch)
{
	switch ( MessageType )
	{
	case EMessageType::Register:
		
		break;
	case EMessageType::RegisterStatus:
		break;
	}
	
}
