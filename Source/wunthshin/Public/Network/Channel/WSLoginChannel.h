#pragma once

#include "CoreMinimal.h"
#include "Network/UUIDWrapper.h"
#include "Network/Channel/WSChannelBase.h"

#include "WSLoginChannel.generated.h"

DEFINE_CHANNEL_MESSAGE(LoginChannel, EMessageChannelType::Login, LoginRequest, EMessageType::Login)
DEFINE_CHANNEL_MESSAGE(LoginChannel, EMessageChannelType::Login, LoginReply, EMessageType::LoginStatus)
DEFINE_CHANNEL_MESSAGE(LoginChannel, EMessageChannelType::Login, LogoutRequest, EMessageType::Logout)
DEFINE_CHANNEL_MESSAGE(LoginChannel, EMessageChannelType::Login, LogoutReply, EMessageType::LogoutOK)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnLoginStatusChangedServer, const bool, bLogin, const uint32, ID, const FUUIDWrapper&, LoginUUID, const uint32, MessageIdentifier);

UCLASS()
class WUNTHSHIN_API UWSLoginChannel : public UWSChannelBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnLoginStatusChangedServer OnLoginStatusChanged;

	virtual void ReceivedBunch(MessageBase& Bunch) override;

protected:
	virtual void SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch) override;

	UPROPERTY()
	TMap<FUUIDWrapper, uint32> SessionIDMap;
};
