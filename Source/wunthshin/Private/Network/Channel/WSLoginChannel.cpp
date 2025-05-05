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
			FUUIDWrapper SessionID{};
			for (size_t i = 0; i < LoginMessage.sessionId.size(); i++)
			{
				SessionID.uuid.Add( static_cast<uint8>( LoginMessage.sessionId[ i ] ) );
			}
			check( SessionID.IsValid() );
			SessionIDMap.Add( SessionID, LoginMessage.id );
			OnLoginStatusChanged.Broadcast( true, LoginMessage.id, SessionID, LoginMessage.messageIdentifier );
		}
		break;
	}
	case EMessageType::LogoutOK:
	{
		LogoutOKMessage& LogoutMessage = reinterpret_cast<LogoutOKMessage&>(Bunch);

		if (LogoutMessage.success)
		{
			FUUIDWrapper SessionID{};
			for (size_t i = 0; i < LogoutMessage.sessionID.size(); i++)
			{
				SessionID.uuid.Add( static_cast<uint8>( LogoutMessage.sessionID[ i ] ) );
			}
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