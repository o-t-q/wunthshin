#pragma once

#include "CoreMinimal.h"
#include "wunthshin/Network/Channel/WSChannelBase.h"

#include "WSRegisterChannel.generated.h"

UENUM()
enum class ERegisterFailCodeUE : uint8
{
	None = (uint8)ERegistrationFailCode::None,
	Name = (uint8)ERegistrationFailCode::Name,
	Email = (uint8)ERegistrationFailCode::Email
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLastRegisterationStatus, bool, bSuccess, ERegisterFailCodeUE, FailCode);

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWSRegisterChannel : public UWSChannelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FLastRegisterationStatus LastRegistrationStatus;

	virtual void ReceivedBunch(MessageBase& Bunch) override;

protected:
	virtual void SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch) override;

};
