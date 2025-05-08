#pragma once
#include "CoreMinimal.h"
#include "Network/WSNetDriver.h"
#include "Network/UUIDWrapper.h"
#include "Network/Channel/WSRegisterChannel.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "WSServerSubsystem.generated.h"

enum class EItemType : uint8;
class AwunthshinPlayerController;
class UWSLoginChannel;
class UWSRegisterChannel;
class UWSItemChannel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAccountInfoChanged, const bool, bLogin, const int32, UserID, const FUUIDWrapper&, SessionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServerSubsystemInitialized);
extern FOnServerSubsystemInitialized GOnServerSubsystemInitialized;

// https://unrealcommunity.wiki/config-files-read-and-write-to-config-files-zuoaht01
UCLASS(Config = Engine)
class WUNTHSHIN_API UWSServerSubsystem : public UGameInstanceSubsystem, public FTickableGameObject, public FNetworkNotify
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnAccountInfoChanged OnAccountInfoChanged;
	
	UFUNCTION(BlueprintCallable)
	UWSLoginChannel* GetLoginChannel() const { return LoginChannel.Get(); }

	UFUNCTION(BlueprintCallable)
	UWSItemChannel* GetItemChannel() const { return ItemChannel.Get(); }

	UFUNCTION(BlueprintCallable)
	UWSRegisterChannel* GetRegisterChannel() const { return RegisterChannel.Get(); }

	// Login Functions
	static bool ValidateLoginRequest( const FString& InID, const TArray<uint8>& HashedPassword );
	bool TrySendLoginRequest(const FString& InID, const FSHA256Signature& HashedPassword) const;
	UFUNCTION(BlueprintCallable)
	bool TrySendLoginRequest(const FString& InID, const FString& InPlainPassword);

	// Logout Functions
	static bool ValidateLogoutRequest( const AwunthshinPlayerController* PlayerController );
	UFUNCTION(BlueprintCallable)
	bool Client_TrySendLogoutRequest();
	bool Server_SendLogoutRequest( const AwunthshinPlayerController* PlayerController ) const;

	// Register Functions
	static bool ValidateRegisterRequest( const FString& InID, const FString& InEmail, const TArray<uint8>& HashedPassword );
	UFUNCTION(BlueprintCallable)
	bool TrySendRegister( const AwunthshinPlayerController* PlayerController, const FString& InID, const FString& InEmail, const FString& InPlainPassword );
	
	// Item request functions
	UFUNCTION(BlueprintCallable)
	bool Client_TryGetItems( const AwunthshinPlayerController* PlayerController, int32 Page ) const;
	bool Server_GetItems( const AwunthshinPlayerController* PlayerController, int32 Page ) const;
	
	UFUNCTION(BlueprintCallable)
	bool TryAddItem(const EItemType ItemType, int32 ItemID, int32 Count);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Hashing")
	FString HashFStringToSHA256(const FString& PlainText);

	AwunthshinPlayerController* GetPlayerController(const FUUIDWrapper& UUID) const;
	
	const FUUIDWrapper* GetUUID( const AwunthshinPlayerController* Player ) const;

protected:
	bool HashPassword(const FString& InPlainPassword, FSHA256Signature& OutSignature, const FString& InSalt = TEXT("")) const;
	
	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;
	
	UPROPERTY(Transient)
	UWSNetDriver* NetDriver = nullptr;
	
	UPROPERTY(Config)
	FString Host;

	UPROPERTY(Config)
	int32 Port;

private:
	UFUNCTION()
	void OnLoginMessageReceived( bool bLogin, uint32 ID, const FUUIDWrapper& LoginUUID, uint32 InPCUniqueID );

	UFUNCTION(BlueprintCallable)
	void ConnectToMiddleware(const FString& InHost, int32 InPort);

	UFUNCTION(BlueprintCallable)
	void ConnectToServer(const FString& InHost, int32 InPort = 17777) const;

	// 클라이언트의 세션 아이디
	UPROPERTY(VisibleAnywhere)
	FUUIDWrapper ClientSessionID;

	// 클라이언트의 유저 아이디
	UPROPERTY(VisibleAnywhere)
	int32 ClientUserID;

	// 서버가 기록한 세션 아이디와 플레이어 컨트롤러 페어
	UPROPERTY(VisibleAnywhere)
	TMap<AwunthshinPlayerController*, FUUIDWrapper> SessionIDs;
	UPROPERTY(VisibleAnywhere)
	TMap<FUUIDWrapper, AwunthshinPlayerController*> PlayerControllers;
	
	TWeakObjectPtr<UWSLoginChannel> LoginChannel = nullptr;

	TWeakObjectPtr<UWSRegisterChannel> RegisterChannel = nullptr;

	TWeakObjectPtr<UWSItemChannel> ItemChannel = nullptr;
};
