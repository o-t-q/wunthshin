#include "Network/Channel/WSLoginChannel.h"

#include "Kismet/GameplayStatics.h"

void UWSLoginChannel::ReceivedBunch(MessageBase& Bunch)
{
	switch (Bunch.GetType())
	{
	case EMessageType::LoginStatus:
	{
		LoginStatusMessage& LoginMessage = reinterpret_cast<LoginStatusMessage&>(Bunch);

		if ( LoginMessage.success )
		{
			const FUUIDWrapper Wrapper( LoginMessage.sessionId );
			check( Wrapper.IsValid() );
			SessionIDMap.Add( Wrapper, LoginMessage.id );
			OnLoginStatusChanged.Broadcast( true, LoginMessage.id, Wrapper, LoginMessage.messageIdentifier );
		}
		break;
	}
	case EMessageType::LogoutOK:
	{
		LogoutOKMessage& LogoutMessage = reinterpret_cast<LogoutOKMessage&>(Bunch);

		if (LogoutMessage.success)
		{
			// todo: Handle the case where a player did not properly logout
			const FUUIDWrapper SessionID( LogoutMessage.sessionID );
			check( SessionID.IsValid() );
			const uint32 ID = SessionIDMap[ SessionID ];
			SessionIDMap.Remove( SessionID );
			OnLoginStatusChanged.Broadcast( false, ID, SessionID, 0 );
		}
			
		break;
	}
	default: check(false);
	}
}

void UWSLoginChannel::SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch)
{
}