#include "WSServerSubsystem.h"
#include "wunthshin/Network/Channel/Public/WSLoginChannel.h"
#include "wunthshin/Network/Channel/Public/WSRegisterChannel.h"
#include "wunthshin/Misc/Public/sha256.h"
#include "message.h"

constexpr static size_t IDSizeLimit = sizeof(decltype(std::declval<LoginMessage>().name._Elems));

bool UWSServerSubsystem::HashPassword(const FString& InPlainPassword, FSHA256Signature& OutSignature, const FString& InSalt) const
{
	const FRegexPattern AlphaNumericalAndSpecial(TEXT(R"(^[\w$&+,:;=?@#|'<>.^*()%!-]*$)"));
	FString Combined = InPlainPassword;

	if (!InSalt.IsEmpty())
	{
		Combined += InSalt;
	}

	if (FRegexMatcher Matcher(AlphaNumericalAndSpecial, Combined); !Matcher.FindNext())
	{
		OutSignature = {};
		return false;
	}

	TStringConversion charArray = StringCast<ANSICHAR>(*Combined);
	SHA256 sha256;
	sha256.init();
	sha256.update( (unsigned char*)charArray.Get(), charArray.Length() );
	sha256.final( OutSignature.Signature );
	return true;
}

bool UWSServerSubsystem::TrySendLoginRequest(const FString& InID, const FSHA256Signature& HashedPassword)
{
	if (!LoginChannel)
	{
		check(false);
		return false;
	}

	if (InID.IsEmpty())
	{
		return false;
	}

	if (!InID.IsEmpty() && (size_t)InID.Len() + 1 >= IDSizeLimit)
	{
		return false; // Comparing as the greater or equal, due to the null character
	}

	static const FRegexPattern Alphanumerical(R"([\w]+)");
	if (FRegexMatcher Match(Alphanumerical, InID); !Match.FindNext())
	{
		return false; // Alphanumerical only
	}

	if (std::ranges::all_of(HashedPassword.Signature, [](const uint8_t& c) { return c == (uint8_t)0; }))
	{
		check(false); // No empty hash password
		return false;
	}
	
	LoginMessage loginMessage;
	const ANSICHAR* charArray = StringCast<ANSICHAR>(*InID).Get();
	std::memcpy( loginMessage.name.data(), charArray, sizeof(InID.Len()) );
	std::memcpy( loginMessage.hashedPassword.data(), HashedPassword.Signature, sizeof(loginMessage.hashedPassword) );
	FNetLoginChannelLoginRequestMessage::Send(NetDriver->ServerConnection, loginMessage);
	return true;
}

bool UWSServerSubsystem::TrySendLoginRequest(const FString& InID, const FString& InPlainPassword)
{
	if (FSHA256Signature Hashed{}; HashPassword(InPlainPassword, Hashed))
	{
		return TrySendLoginRequest(InID, Hashed);
	}

	return false;
}

bool UWSServerSubsystem::TrySendLogoutRequest()
{
	if (!LoginChannel)
	{
		check(false);
		return false;
	}

	if (!LoginChannel->HasLogin())
	{
		return false;
	}

	FUUIDWrapper sessionId = LoginChannel->GetSessionID();
	
	if (!sessionId.IsValid())
	{
		check(false);
		return false;
	}

	LogoutMessage logoutMessage( std::move( sessionId.uuid ) );
	FNetLoginChannelLogoutRequestMessage::Send(NetDriver->ServerConnection, logoutMessage);

	return true;
}

bool UWSServerSubsystem::TrySendRegister(const FString& InID, const FString& InEmail, const FString& InPlainPassword)
{
	if (FSHA256Signature Hashed{}; HashPassword(InPlainPassword, Hashed))
	{
		return TrySendRegister(InID, InEmail, Hashed);
	}

	return false;
}

void UWSServerSubsystem::ConnectToServer(const FString& InHost, int32 InPort)
{
	NetDriver = NewObject<UWSNetDriver>( this, TEXT( "WS NetDriver" ) );
	FURL URL{};
	URL.Host = InHost.IsEmpty() ? Host : InHost;
	URL.Port = InPort == -1 ? Port : InPort;
	URL.Protocol = "tcp";
	URL.Map = "";

	check( !URL.Host.IsEmpty() );
	check( URL.Port != -1 );

	FString OutError;
	if ( !NetDriver->InitConnect( this, URL, OutError ) )
	{
		check(false);
		UE_LOG(LogTemp, Error, TEXT("Connect Failed"));
		return;
	}

#if !UE_SERVER
	LoginChannel = Cast<UWSLoginChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Login]);
	RegisterChannel = Cast<UWSRegisterChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Register]);
#endif
}

void UWSServerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ConnectToServer(Host, Port);
}

bool UWSServerSubsystem::TrySendRegister(const FString& InID, const FString& InEmail, const FSHA256Signature& HashedPassword)
{
	if (!RegisterChannel)
	{
		check(false);
		return false;
	}

	if (InID.IsEmpty())
	{
		return false;
	}

	if (InEmail.IsEmpty())
	{
		return false;
	}

	if (!InID.IsEmpty() && (size_t)InID.Len() + 1 >= IDSizeLimit)
	{
		return false; // Comparing as the greater or equal, due to the null character
	}

	static const FRegexPattern Alphanumerical(R"([\w]+)");
	if (FRegexMatcher Match(Alphanumerical, InID); !Match.FindNext())
	{
		return false; // Alphanumerical only
	}

	static const FRegexPattern EmailFormat(R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)");
	if (FRegexMatcher Match(EmailFormat, InEmail); !Match.FindNext())
	{
		return false; // Invalid email format
	}

	if (std::ranges::all_of(HashedPassword.Signature, [](const uint8_t& c) { return c == (uint8_t)0; }))
	{
		check(false); // No empty hash password
		return false;
	}

	RegisterMessage registerMessage{};
	TStringConversion nameArray = StringCast<ANSICHAR>(*InID);
	TStringConversion emailArray = StringCast<ANSICHAR>(*InEmail);
	std::memcpy(registerMessage.name.data(), nameArray.Get(), nameArray.Length());
	std::memcpy(registerMessage.email.data(), emailArray.Get(), emailArray.Length());
	std::memcpy(registerMessage.hashedPassword.data(), HashedPassword.Signature, sizeof(registerMessage.hashedPassword));
	FNetRegisterChannelRegisterRequestMessage::Send(NetDriver->ServerConnection, registerMessage);
	return true;
}

void UWSServerSubsystem::Tick(float DeltaTime)
{
	if (NetDriver)
	{
		NetDriver->TickDispatch(DeltaTime);
	}
}

bool UWSServerSubsystem::IsTickable() const
{
	return true;
}

TStatId UWSServerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWSServerSubsystem, STATGROUP_Tickables);
}
