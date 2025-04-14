#pragma once
#include "CoreMinimal.h"
#include "../WSNetDriver.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "WSServerSubsystem.generated.h"

struct FUUIDWrapper;
class UWSLoginChannel;
class UWSRegisterChannel;
class UWSItemChannel;

// https://unrealcommunity.wiki/config-files-read-and-write-to-config-files-zuoaht01
UCLASS(Config = Engine)
class WUNTHSHIN_API UWSServerSubsystem : public UGameInstanceSubsystem, public FTickableGameObject, public FNetworkNotify
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UWSLoginChannel* GetLoginChannel() { return LoginChannel; }

	UFUNCTION(BlueprintCallable)
	UWSItemChannel* GetItemChannel() { return ItemChannel; }

	UFUNCTION(BlueprintCallable)
  UWSRegisterChannel* GetRegisterChannel() { return RegisterChannel; }

	bool HashPassword(const FString& InPlainPassword, FSHA256Signature& OutSignature, const FString& InSalt = TEXT("")) const;

	bool TrySendLoginRequest(const FString& InID, const FSHA256Signature& HashedPassword);

	bool TrySendRegister(const FString& InID, const FString& InEmail, const FSHA256Signature& HashedPassword);

	UFUNCTION(BlueprintCallable)
	bool TrySendLoginRequest(const FString& InID, const FString& InPlainPassword);

	UFUNCTION(BlueprintCallable)
	bool TrySendLogoutRequest();

	UFUNCTION(BlueprintCallable)
	bool TrySendRegister(const FString& InID, const FString& InEmail, const FString& InPlainPassword);

	UFUNCTION(BlueprintCallable)
	void ConnectToServer(const FString& InHost, int32 InPort);

	UFUNCTION(BlueprintCallable)
	bool TryAddItem(const EItemType ItemType, int32 ItemID, int32 Count);

	UFUNCTION(BlueprintCallable)
	bool TryGetItems(int32 Page);

	FUUIDWrapper GetSessionID() const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Hashing")
	FString HashFStringToSHA256(const FString& PlainText);

protected:
	void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

	TStatId GetStatId() const override;

	UPROPERTY(Transient)
	UWSNetDriver* NetDriver = nullptr;

	UPROPERTY(Config)
	FString Host;

	UPROPERTY(Config)
	int32 Port;

private:
	UWSLoginChannel* LoginChannel = nullptr;

	UWSRegisterChannel* RegisterChannel = nullptr;

	UWSItemChannel* ItemChannel = nullptr;
};
