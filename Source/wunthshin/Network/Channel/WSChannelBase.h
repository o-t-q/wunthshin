#pragma once
#include "CoreMinimal.h"
#include "Engine/Channel.h"
#include "WSChannelBase.generated.h"

UCLASS()
class WUNTHSHIN_API UWSChannelBase : public UChannel
{
	GENERATED_BODY()
public:
	// virtual void SendBunch(FOutBunch& Bunch);
	// virtual void ReceivedBunch(FInBunch& Bunch) override;
};
