#pragma once

#include "CoreMinimal.h"
#include "Network/Channel/WSChannelBase.h"

#include "WSLoginChannel.generated.h"

DEFINE_CHANNEL_MESSAGE(LoginChannel, EMessageChannelType::Login, LoginRequest, EMessageType::Login)
DEFINE_CHANNEL_MESSAGE(LoginChannel, EMessageChannelType::Login, LoginReply, EMessageType::LoginStatus)
DEFINE_CHANNEL_MESSAGE(LoginChannel, EMessageChannelType::Login, LogoutRequest, EMessageType::Logout)
DEFINE_CHANNEL_MESSAGE(LoginChannel, EMessageChannelType::Login, LogoutReply, EMessageType::LogoutOK)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginStatusChanged, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLastLoginStatusDelegate, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLastLogoutStatusDelegate, bool, bSuccess);

USTRUCT(BlueprintType)
struct FUUIDWrapper
{
	GENERATED_BODY()

	UUID uuid{};

	bool IsValid() const
	{
		return !std::ranges::all_of(uuid, [](const std::byte& b) {return b == (std::byte)0; });
	}
};

UCLASS()
class WUNTHSHIN_API UWSLoginChannel : public UWSChannelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnLoginStatusChanged OnLoginStatusChanged;

	UPROPERTY(BlueprintAssignable)
	FLastLoginStatusDelegate LastLoginStatusDelegate;

	UPROPERTY(BlueprintAssignable)
	FLastLogoutStatusDelegate LastLogoutStatusDelegate;

	UFUNCTION(BlueprintCallable)
	bool HasLogin() const { return bLogin; }

	UFUNCTION(BlueprintCallable)
	const FString& GetID() const { return LoginId; }

	UFUNCTION(BlueprintCallable)
	FUUIDWrapper GetSessionID() const { return SessionId; }

	virtual void ReceivedBunch(MessageBase& Bunch) override;

protected:
	virtual void SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch) override;

private:
	bool bLogin = false;
	FString LoginId = "";
	FUUIDWrapper SessionId;

};
