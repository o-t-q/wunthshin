#include "wunthshin/Network/Channel/Public/WSLoginChannel.h"

void UWSLoginChannel::ReceivedBunch(MessageBase& Bunch)
{
	switch (Bunch.GetType())
	{
	case EMessageType::LoginStatus:
	{
		LoginStatusMessage& loginMessage = reinterpret_cast<LoginStatusMessage&>(Bunch);
		LastLoginStatusDelegate.Broadcast(loginMessage.success);

		if ( loginMessage.success )
		{
			std::ranges::copy( loginMessage.sessionId, SessionId.uuid.begin() );
			check( SessionId.IsValid() );
			bLogin = true;
			OnLoginStatusChanged.Broadcast(bLogin);
		}

		if ( !loginMessage.success && !bLogin )
		{
			LoginId = "";
		}
		break;
	}
	case EMessageType::LogoutOK:
	{
		if (!bLogin)
		{
			break;
		}

		LogoutOKMessage& logoutMessage = reinterpret_cast<LogoutOKMessage&>(Bunch);
		LastLogoutStatusDelegate.Broadcast(logoutMessage.success);

		if (logoutMessage.success)
		{
			std::ranges::fill( SessionId.uuid, (std::byte)0 );
			LoginId = "";
			bLogin = false;
			OnLoginStatusChanged.Broadcast(bLogin);
		}
		break;
	}
	default: check(false);
	}
}

void UWSLoginChannel::SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch)
{
	if ( MessageType == EMessageType::Login && !bLogin )
	{
		LoginMessage& loginMessage = reinterpret_cast<LoginMessage&>(Bunch);
		LoginId = FString( loginMessage.name.data() );
	}

	if ( MessageType == EMessageType::Logout && bLogin )
	{
		LogoutMessage& logoutMessage = reinterpret_cast<LogoutMessage&>(Bunch);
		std::ranges::copy( SessionId.uuid, logoutMessage.sessionId.begin());
	}
}