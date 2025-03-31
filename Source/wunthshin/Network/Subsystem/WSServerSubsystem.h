#pragma once
#include "CoreMinimal.h"
#include "../WSNetDriver.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WSServerSubsystem.generated.h"

class UWSLoginChannel;

UCLASS(config=wunthshin)
class WUNTHSHIN_API UWSServerSubsystem : public UGameInstanceSubsystem, public FTickableGameObject, public FNetworkNotify
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UWSLoginChannel* GetLoginChannel() { return LoginChannel; }

	bool HashPassword(const FString& InPlainPassword, FSHA256Signature& OutSignature, const FString& InSalt = TEXT("")) const;

	bool TrySendLoginRequest(const FString& InID, const FSHA256Signature& HashedPassword);

	UFUNCTION(BlueprintCallable)
	bool TrySendLoginRequest(const FString& InID, const FString& InPlainPassword);

	UFUNCTION(BlueprintCallable)
	bool TrySendLogoutRequest();

protected:
	UPROPERTY(Transient)
	UWSNetDriver* NetDriver = nullptr;

	UPROPERTY(Config)
	FString Host;

	UPROPERTY(Config)
	int32 Port;

private:
	UWSLoginChannel* LoginChannel = nullptr;
	

	// FTickableGameObject을(를) 통해 상속됨
	void Tick(float DeltaTime) override;

	TStatId GetStatId() const override;

};
