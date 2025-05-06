#include "Network/Subsystem/WSServerSubsystem.h"
#include "Network/Channel/WSLoginChannel.h"
#include "Network/Channel/WSRegisterChannel.h"
#include "Network/Channel/WSItemChannel.h"
#include "Misc/sha256.h"
#include "message.h"
#include "Controller/wunthshinPlayerController.h"
#include "Misc/Paths.h"
#include "Misc/outputdeviceNull.h"

FOnServerSubsystemInitialized GOnServerSubsystemInitialized;
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
	SHA256 SHA256;
	SHA256.init();
	SHA256.update( (unsigned char*)charArray.Get(), charArray.Length() );
	SHA256.final( OutSignature.Signature );
	return true;
}

void UWSServerSubsystem::ConnectToMiddleware(const FString& InHost, int32 InPort)
{
	if (GetWorld()->GetNetMode() != NM_Client)
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

		LoginChannel = Cast<UWSLoginChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Login]);
		RegisterChannel = Cast<UWSRegisterChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Register]);
		ItemChannel = Cast<UWSItemChannel>(NetDriver->ServerConnection->Channels[(uint8)EMessageChannelType::Item]);	
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
	if ( GetWorld()->GetNetMode() != NM_Client )
	{
		ConnectToMiddleware( Host, Port );
		GOnServerSubsystemInitialized.Broadcast();

		// Replicates the session id, user id, and login status to the designated client.
		LoginChannel->OnLoginStatusChanged.AddUniqueDynamic( this, &UWSServerSubsystem::OnLoginMessageReceived );
		RegisterChannel->LastRegistrationStatus.AddUniqueDynamic( this, &UWSServerSubsystem::OnRegisterMessageReceived );
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

bool UWSServerSubsystem::Client_TrySendLoginRequest( const FString& InID, const FSHA256Signature& HashedPassword ) const
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

	AwunthshinPlayerController* PC = Cast<AwunthshinPlayerController>( GetWorld()->GetFirstPlayerController() );
	check(PC);
	
	if (PC)
	{
		PC->Server_SendLoginRequest( InID, HashedPasswordArray );
		return true;
	}
	return false;
}

bool UWSServerSubsystem::Client_TrySendLoginRequest( const FString& InID, const FString& InPlainPassword )
{
	if ( FSHA256Signature Hashed{}; HashPassword( InPlainPassword, Hashed ) )
	{
		return Client_TrySendLoginRequest( InID, Hashed );
	}
	return false;
}

bool UWSServerSubsystem::Server_SendLoginRequest( const AwunthshinPlayerController* PlayerController,  const FString& InID, const FSHA256Signature& HashedPassword ) const
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		check(false); // Client should use the client function
		return false;
	}
	
	if ( !LoginChannel.IsValid() )
	{
		check(false);
		return false;
	}

	TArray<uint8> HashedPasswordArray;
	for (size_t i = 0; i < std::size(HashedPassword.Signature); i++)
	{
		HashedPasswordArray.Add( HashedPassword.Signature[i] );
	}

	if (!ValidateLoginRequest( InID, HashedPasswordArray ))
	{
		return false;
	}
	
	LoginMessage LoginMessage;
	const TStringConversion CharArray = StringCast<ANSICHAR>(*InID);
	std::memcpy( LoginMessage.name.data(), CharArray.Get(), CharArray.Length() );
	std::memcpy( LoginMessage.hashedPassword.data(), HashedPassword.Signature, sizeof(LoginMessage.hashedPassword) );
	LoginMessage.messageIdentifier = PlayerController->GetUniqueID();
	FNetLoginChannelLoginRequestMessage::Send( NetDriver->ServerConnection, LoginMessage );
	return true;
}

bool UWSServerSubsystem::Server_SendRegister(const AwunthshinPlayerController* PlayerController, const FString& InID, const FString& InEmail, const FSHA256Signature& HashedPassword) const
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		check(false); // Client should use the player controller
		return false;
	}

	if (!RegisterChannel.IsValid())
	{
		check(false);
		return false;
	}

	TArray<uint8> HashedPasswordArray;
	for (size_t i = 0; i < std::size(HashedPassword.Signature); i++)
	{
		HashedPasswordArray.Add( HashedPassword.Signature[i] );
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
	std::memcpy(registerMessage.hashedPassword.data(), HashedPassword.Signature, sizeof(registerMessage.hashedPassword));
	registerMessage.messageIdentifier = PlayerController->GetUniqueID();
	FNetRegisterChannelRegisterRequestMessage::Send(NetDriver->ServerConnection, registerMessage);
	return true;
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
	for ( size_t i = 0; i < 32; ++i )
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

bool UWSServerSubsystem::Client_TrySendRegister( const AwunthshinPlayerController* PlayerController,
	const FString& InID, const FString& InEmail, const FString& InPlainPassword )
{
	if (FSHA256Signature Hashed{}; HashPassword(InPlainPassword, Hashed))
	{
		TArray<uint8> HashedPasswordArray;
		for (size_t i = 0; i < std::size(Hashed.Signature); ++i)
		{
			HashedPasswordArray.Add( Hashed.Signature[ i ] );
		}
		
		PlayerController->Server_SendRegister(InID, InEmail, HashedPasswordArray);
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

void UWSServerSubsystem::OnLoginMessageReceived( const bool bLogin, const uint32 ID, const FUUIDWrapper& LoginUUID, const uint32 InPCUniqueID )
{
	if ( bLogin ) // Login
	{
		for ( auto Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
		{
			if ( AwunthshinPlayerController* Casted = Cast<AwunthshinPlayerController>(*Iterator);
				 Casted && Casted->GetUniqueID() == InPCUniqueID )
			{
				Casted->bLogin = bLogin;
				Casted->SetUserID( ID );
				Casted->SessionID = LoginUUID;

				SessionIDs.Emplace( Casted, LoginUUID );
				PlayerControllers.Emplace( LoginUUID, Casted );
				break;
			}
		}
	}
	else // Logout
	{
		const auto& ToRemove = SessionIDs.FilterByPredicate( [ &LoginUUID ]( const TPair<AwunthshinPlayerController*, FUUIDWrapper>& Pair )
		{
			return Pair.Value == LoginUUID;
		} );

		for (const TPair<AwunthshinPlayerController*, FUUIDWrapper>& Removal : ToRemove)
		{
			Removal.Key->bLogin= false;
			Removal.Key->SetUserID( 0 );
			Removal.Key->SessionID = {};
			
			SessionIDs.Remove( Removal.Key );
			PlayerControllers.Remove( Removal.Value );
		}
	}
}

void UWSServerSubsystem::OnRegisterMessageReceived( const uint32 InPCUniqueID, bool bSuccess,
	const ERegisterFailCodeUE FailCode )
{
	for ( auto Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		if ( AwunthshinPlayerController* Casted = Cast<AwunthshinPlayerController>(*Iterator);
			 Casted && Casted->GetUniqueID() == InPCUniqueID )
		{
			Casted->Client_PropagateRegisterStatus( bSuccess, FailCode );
			break;
		}
	}
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
