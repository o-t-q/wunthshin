#pragma once
#include "CoreMinimal.h"
#include "../WSNetDriver.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WSServerSubsystem.generated.h"


UCLASS(config=wunthshin)
class WUNTHSHIN_API UWSServerSubsystem : public UGameInstanceSubsystem, public FNetworkNotify
{
	GENERATED_BODY()
public:
	
protected:
	UPROPERTY(Transient)
	UWSNetDriver* NetDriver = nullptr;

	UPROPERTY(Config)
	FString Host;

	UPROPERTY(Config)
	int32 Port;

protected:
	
};
