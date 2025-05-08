#pragma once

#include "CoreMinimal.h"
#include "Network/Channel/WSChannelBase.h"

#include "WSRegisterChannel.generated.h"

DEFINE_CHANNEL_MESSAGE(RegisterChannel, EMessageChannelType::Register, RegisterRequest, EMessageType::Register);
DEFINE_CHANNEL_MESSAGE(RegisterChannel, EMessageChannelType::Register, RegisterReply, EMessageType::RegisterStatus);

UENUM(BlueprintType)
enum class ERegisterFailCodeUE : uint8
{
	None = (uint8)ERegistrationFailCode::None,
	Name = (uint8)ERegistrationFailCode::Name,
	Email = (uint8)ERegistrationFailCode::Email
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FLastRegisterationStatusServer, const uint32, InPCUniqueID, bool, bSuccess, ERegisterFailCodeUE, FailCode);

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWSRegisterChannel : public UWSChannelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FLastRegisterationStatusServer LastRegistrationStatus;

	virtual void ReceivedBunch(MessageBase& Bunch) override;

protected:
	virtual void SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch) override;

};
