#include "Network/Subsystem/WSServerSubsystem.h"
#include "Network/Channel/WSLoginChannel.h"
#include "Network/Channel/WSRegisterChannel.h"
#include "Network/Channel/WSItemChannel.h"
#include "Misc/sha256.h"
#include "message.h"
#include "Misc/Paths.h"
#include "Misc/outputdeviceNull.h"

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
	TStringConversion charArray = StringCast<ANSICHAR>(*InID);
	std::memcpy( loginMessage.name.data(), charArray.Get(), charArray.Length() );
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
	ItemChannel = Cast<UWSItemChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Item]);
#endif
}

bool UWSServerSubsystem::TryAddItem( const EItemType ItemType, int32 ItemID, int32 Count )
{
	AddItemRequestMessage Message({}, (EDBItemType)ItemType, ItemID, Count);
	FNetItemChannelAddItemMessage::Send( NetDriver->ServerConnection, Message );
	return true;
}

bool UWSServerSubsystem::TryGetItems( int32 Page )
{
	GetItemsRequestMessage ItemRequest(Page, {});
	FNetItemChannelGetItemsMessage::Send( NetDriver->ServerConnection, ItemRequest );
	return true;
}

FUUIDWrapper UWSServerSubsystem::GetSessionID() const
{
	UUID returnValue{};
	if (LoginChannel)
	{
		check(LoginChannel->HasLogin());
		FUUIDWrapper SessionID = LoginChannel->GetSessionID();

		if (LoginChannel->HasLogin() && SessionID.IsValid())
		{
			std::copy(SessionID.uuid.begin(), SessionID.uuid.end(), returnValue.begin());
		}
	}

	return FUUIDWrapper(returnValue);
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


// FString을 std::string으로 변환하는 함수
std::string FStringToStdString(const FString& FStringInput)
{
	// FString을 UTF-8로 변환
	std::string StdStringInput(TCHAR_TO_UTF8(*FStringInput));
	return StdStringInput;
}

// SHA256 해시 생성 함수
FString UWSServerSubsystem::HashFStringToSHA256(const FString& PlainText)
{
	// FString을 std::string으로 변환
	std::string stdStringInput = FStringToStdString(PlainText);

	// SHA256 객체 생성
	SHA256 sha256;

	// SHA256 초기화
	sha256.init();

	// 데이터를 해시 함수에 추가
	sha256.update(reinterpret_cast<const unsigned char*>(stdStringInput.c_str()), stdStringInput.length());

	// 해시 계산
	unsigned char hashOutput[SHA256::DIGEST_SIZE];
	sha256.final(hashOutput);

	// 해시값을 FString으로 변환
	FString HashString;
	for (int i = 0; i < SHA256::DIGEST_SIZE; ++i)
	{
		HashString += FString::Printf(TEXT("%02x"), hashOutput[i]);
	}

	return HashString;
}