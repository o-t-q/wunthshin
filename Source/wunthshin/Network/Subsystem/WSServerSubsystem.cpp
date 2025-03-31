#include "WSServerSubsystem.h"
#include "wunthshin/Network/Channel/Public/WSLoginChannel.h"
#include "message.h"

bool UWSServerSubsystem::HashPassword(const FString& InPlainPassword, FSHA256Signature& OutSignature, const FString& InSalt) const
{
	static const FRegexPattern AlphaNumericalAndSpecial(R"("^[a-zA-Z0-9!@#$&()\\-`.+,/\"]*$")");
	FString Combined = InPlainPassword;

	if (!InSalt.IsEmpty())
	{
		Combined += InSalt;
	}

	if (FRegexMatcher Matcher(AlphaNumericalAndSpecial, InPlainPassword); !Matcher.FindNext())
	{
		OutSignature = {};
		return false;
	}

	TStringConversion charArray = StringCast<ANSICHAR>(*Combined);
	
	if (!FGenericPlatformMisc::GetSHA256Signature(charArray.Get(), charArray.Length(), OutSignature))
	{
		check(false);
		OutSignature = {};
		return false;
	}

	return true;
}

bool UWSServerSubsystem::TrySendLoginRequest(const FString& InID, const FSHA256Signature& HashedPassword)
{
	constexpr static size_t IDSizeLimit = sizeof(decltype(std::declval<LoginMessage>().name._Elems));

	if (!InID.IsEmpty() && (size_t)InID.Len() + 1 >= IDSizeLimit)
	{
		return false; // Comparing as the greater or equal, due to the null character
	}

	static const FRegexPattern Alphanumerical(R"([A-Za-z0-9]+)");
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

void UWSServerSubsystem::Tick(float DeltaTime)
{
}

TStatId UWSServerSubsystem::GetStatId() const
{
	return TStatId();
}
