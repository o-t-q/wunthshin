#pragma once
#include "Network/Channel/WSChannelBase.h"
#include "WSPlayerStateChannel.generated.h"

class AwunthshinPlayerController;
//------------------------------------------------------------------------------------
// Message Define
//
#pragma region [Message] 
DEFINE_CHANNEL_MESSAGE(PlayerStateChannel, EMessageChannelType::PlayerState, CharacterStatus, EMessageType::CharacterStatus);
#pragma endregion [Message]

//------------------------------------------------------------------------------------
// Event Delegate
//
#pragma region [Event]
//DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnChangedCharacterStat, int32, StatKey, int32, StatIncreasement);
#pragma endregion [Event]

//------------------------------------------------------------------------------------
// UWSPlayerStateChannel
//
//	- 유저의 정보 업데이트(계정정보, 레벨 등)
//	- 유저가 현재 조작중인 캐릭터의 정보 업데이트 (채널 따로 분리할까 고민중)
//
UCLASS()
class WUNTHSHIN_API UWSPlayerStateChannel : public UWSChannelBase
{
	GENERATED_BODY()

public:
	UWSPlayerStateChannel();

public:
	virtual void ReceivedBunch(MessageBase& Bunch) override;
protected:
	virtual void SendBunchInternal(const EMessageType MessageType, MessageBase& Bunch) override;

private:
	UPROPERTY()
	AwunthshinPlayerController* CurrentPlayerController;
};
