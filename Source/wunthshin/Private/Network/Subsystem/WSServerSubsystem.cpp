#include "Network/Subsystem/WSServerSubsystem.h"
#include "Network/Channel/WSLoginChannel.h"
#include "Network/Channel/WSRegisterChannel.h"
#include "Network/Channel/WSItemChannel.h"
#include "Misc/sha256.h"
#include "message.h"
#include "Controller/wunthshinPlayerController.h"
#include "Engine/DemoNetConnection.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "wunthshin/wunthshin.h"

FOnServerSubsystemInitialized GOnServerSubsystemInitialized;

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
	SHA256 SHA256;
	SHA256.init();
	SHA256.update( (unsigned char*)charArray.Get(), charArray.Length() );
	SHA256.final( OutSignature.Signature );
	return true;
}

void UWSServerSubsystem::ConnectToMiddleware(const FString& InHost, int32 InPort)
{
	NetDriver = NewObject<UWSNetDriver>( this, TEXT( "WS NetDriver" ) );
	FURL URL{};
	URL.Host = InHost.IsEmpty() ? Host : InHost;
	URL.Port = InPort == 0 ? Port : InPort;
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
	
	LoginChannel = Cast<UWSLoginChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Login]);
	RegisterChannel = Cast<UWSRegisterChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Register]);

	// 게임에 접속하지 않고도 메세지를 보냄으로써 아이템 생성 및 조회가 가능할 수 있음
	// 예를 들어, 로그인 API를 통해서 세션 아이디를 부여받고 그 세션 아이디를 이용해서
	// 아이템 채널에 아이템을 추가하라는 메세지를 보낼 수 있고, 이때 데이터베이스 서버는
	// 이 아이템이 게임에 실제로 존재하는지를 알 수가 없음
	// todo: 데이터베이스 서버에서 화이트리스트 IP를 구성해서 메세지를 필터링
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		ItemChannel = Cast<UWSItemChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Item]);	
	}	
}

void UWSServerSubsystem::ConnectToServer(const FString& InHost, const int32 InPort) const
{
	const FString URL = FString::Printf( TEXT("%s:%d"), *(InHost.IsEmpty() ? Host : InHost), InPort == 0 ? Port : InPort );
	
	AwunthshinPlayerController* PlayerController = Cast<AwunthshinPlayerController>( GetWorld()->GetFirstPlayerController() );
	check( PlayerController );
	if ( PlayerController )
	{
		PlayerController->ClientTravel(*URL, TRAVEL_Absolute);
		PlayerController->Server_Authenticate(ClientUserID, ClientSessionID);
	}
}

bool UWSServerSubsystem::TryAddItem( const EItemType ItemType, int32 ItemID, int32 Count )
{
	AddItemRequestMessage Message({}, (EDBItemType)ItemType, ItemID, Count);
	FNetItemChannelAddItemMessage::Send( NetDriver->ServerConnection, Message );
	return true;
}

bool UWSServerSubsystem::Client_TryGetItems( const AwunthshinPlayerController* PlayerController, int32 Page ) const
{
	if ( GetWorld()->GetNetMode() != NM_Client )
	{
		return false;
	}

	if (PlayerController != GetWorld()->GetFirstPlayerController())
	{
		return false;
	}

	PlayerController->Server_UpdateInventory( Page );
	return true;
}

bool UWSServerSubsystem::Server_GetItems( const AwunthshinPlayerController* PlayerController, int32 Page ) const
{
	const UUID UUID = PlayerController->SessionID;
	GetItemsRequestMessage ItemRequest( Page, UUID );
	FNetItemChannelGetItemsMessage::Send( NetDriver->ServerConnection, ItemRequest );
	return true;
}

void UWSServerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ConnectToMiddleware( Host, Port );
	GOnServerSubsystemInitialized.Broadcast();

	if ( LoginChannel.IsValid() )
	{
		LoginChannel->OnLoginStatusChanged.AddUniqueDynamic( this, &UWSServerSubsystem::OnLoginMessageReceived );
	}
}

bool UWSServerSubsystem::ValidateLoginRequest( const FString& InID, const TArray<uint8>& HashedPassword )
{
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

	if (std::all_of(HashedPassword.begin(), HashedPassword.end(), [](const uint8_t& c) { return c == (uint8_t)0; }))
	{
		check(false); // No empty hash password
		return false;
	}
	
	return true;
}

bool UWSServerSubsystem::TrySendLoginRequest( const FString& InID, const FSHA256Signature& HashedPassword ) const
{
	TArray<uint8> HashedPasswordArray;
	for (size_t i = 0; i < std::size(HashedPassword.Signature); i++)
	{
		HashedPasswordArray.Add( HashedPassword.Signature[i] );
	}
	
	if (!ValidateLoginRequest( InID, HashedPasswordArray ))
	{
		return false;
	}

	if ( !LoginChannel.IsValid() )
	{
		check(false);
		return false;
	}
	
	LoginMessage LoginMessage;
	const TStringConversion CharArray = StringCast<ANSICHAR>(*InID);
	std::memcpy( LoginMessage.name.data(), CharArray.Get(), CharArray.Length() );
	std::memcpy( LoginMessage.hashedPassword.data(), HashedPassword.Signature, sizeof(LoginMessage.hashedPassword) );
	LoginMessage.messageIdentifier = GetWorld()->GetFirstPlayerController()->GetUniqueID();
	FNetLoginChannelLoginRequestMessage::Send( NetDriver->ServerConnection, LoginMessage );
	return true;
}

bool UWSServerSubsystem::TrySendLoginRequest( const FString& InID, const FString& InPlainPassword )
{
	if ( FSHA256Signature Hashed{}; HashPassword( InPlainPassword, Hashed ) )
	{
		return TrySendLoginRequest( InID, Hashed );
	}
	return false;
}

bool UWSServerSubsystem::ValidateLogoutRequest( const AwunthshinPlayerController* PlayerController )
{
	if ( !PlayerController )
	{
		return false;
	}
	
	if ( !PlayerController->SessionID.IsValid() )
	{
		return false;
	}

	if ( !PlayerController->bLogin )
	{
		return false;
	}

	return true;
}

bool UWSServerSubsystem::Client_TrySendLogoutRequest()
{
	AwunthshinPlayerController* PC = Cast<AwunthshinPlayerController>( GetWorld()->GetFirstPlayerController() );
	check(PC);

	if (!ValidateLogoutRequest( PC ))
	{
		return false;
	}
	
	if (PC)
	{
		PC->Server_SendLogoutRequest();
		return true;
	}
	return false;
}

bool UWSServerSubsystem::Server_SendLogoutRequest( const AwunthshinPlayerController* PlayerController ) const
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		check( false ); // Client should use the player controller
		return false;
	}

	if (!ValidateLogoutRequest( PlayerController ))
	{
		return false;
	}

	UUID NativeUUID{};
	for ( size_t i = 0; i < 16; ++i )
	{
		NativeUUID[ i ] = static_cast<std::byte>( PlayerController->SessionID.uuid[ i ] );
	}

	LogoutMessage LogoutMessage( NativeUUID );
	FNetLoginChannelLogoutRequestMessage::Send( NetDriver->ServerConnection, LogoutMessage );
	return true;
}

bool UWSServerSubsystem::ValidateRegisterRequest( const FString& InID, const FString& InEmail,
	const TArray<uint8>& HashedPassword )
{
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

	if (std::all_of( HashedPassword.begin(), HashedPassword.end(), [](const uint8_t& c) { return c == (uint8_t)0; }))
	{
		return false;
	}

	return true;
}

bool UWSServerSubsystem::TrySendRegister( const AwunthshinPlayerController* PlayerController,
	const FString& InID, const FString& InEmail, const FString& InPlainPassword )
{
	if (FSHA256Signature Hashed{}; HashPassword(InPlainPassword, Hashed))
	{
		TArray<uint8> HashedPasswordArray;
		for (size_t i = 0; i < std::size(Hashed.Signature); ++i)
		{
			HashedPasswordArray.Add( Hashed.Signature[ i ] );
		}
		
		if (!RegisterChannel.IsValid())
		{
			check(false);
			return false;
		}
		
		if (!ValidateRegisterRequest( InID, InEmail, HashedPasswordArray ))
		{
			return false;
		}

		RegisterMessage registerMessage{};
		TStringConversion nameArray = StringCast<ANSICHAR>(*InID);
		TStringConversion emailArray = StringCast<ANSICHAR>(*InEmail);
		std::memcpy(registerMessage.name.data(), nameArray.Get(), nameArray.Length());
		std::memcpy(registerMessage.email.data(), emailArray.Get(), emailArray.Length());
		std::memcpy(registerMessage.hashedPassword.data(), HashedPasswordArray.GetData(), sizeof(registerMessage.hashedPassword));
		registerMessage.messageIdentifier = PlayerController->GetUniqueID();
		FNetRegisterChannelRegisterRequestMessage::Send(NetDriver->ServerConnection, registerMessage);
		return true;
	}

	return false;
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

void UWSServerSubsystem::OnLoginMessageReceived( const bool bLogin, const uint32 ID, const FUUIDWrapper& InLoginUUID, const uint32 InPCUniqueID )
{
	AwunthshinPlayerController* PlayerController = Cast<AwunthshinPlayerController>(GetWorld()->GetFirstPlayerController());
	if ( PlayerController->GetUniqueID() != InPCUniqueID )
	{
		RequestEngineExit("Invalid Player Controller ID has been respond from server");
	}
	
	if ( bLogin ) // Login
	{
		ClientUserID = ID;
		ClientSessionID = InLoginUUID;
	}
	else // Logout
	{
		ClientUserID = -1;
		ClientSessionID = {};
	}

	OnAccountInfoChanged.Broadcast(bLogin, ClientUserID, ClientSessionID);
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

AwunthshinPlayerController* UWSServerSubsystem::GetPlayerController( const FUUIDWrapper& UUID ) const
{
	if (PlayerControllers.Contains( UUID ))
	{
		return PlayerControllers[UUID];	
	}

	return nullptr;
}

const FUUIDWrapper* UWSServerSubsystem::GetUUID( const AwunthshinPlayerController* Player ) const
{
	if (SessionIDs.Contains( Player ))
	{
		return &SessionIDs[Player];
	}

	return nullptr;
}
