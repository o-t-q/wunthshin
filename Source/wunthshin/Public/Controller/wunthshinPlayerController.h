// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Network/Channel/WSLoginChannel.h"
#include "wunthshinPlayerController.generated.h"

class AWSSharedInventory;
enum class ERegisterFailCodeUE : uint8;
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnLoginStatusChanged );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FLastRegisterationStatus, bool, bSuccess, ERegisterFailCodeUE, FailCode );

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AwunthshinPlayerController : public APlayerController
{
	GENERATED_BODY()
	friend class UWSServerSubsystem; // Updates the UserID, SessionID
	friend class AwunthshinGameMode; // Sending the Logout Request
	friend class UWSItemSubsystem; // Item related message request
	
public:
	AwunthshinPlayerController();

	UPROPERTY(BlueprintAssignable)
	FOnLoginStatusChanged OnLoginStatusChanged;
	
	UPROPERTY(BlueprintAssignable)
	FLastRegisterationStatus LastRegisterationStatus;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetUserID( uint32 InUserID );
	
	UFUNCTION()
	void OnRep_Login() const;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendLoginRequest( const FString& InID, const TArray<uint8>& HashedPassword );

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendLogoutRequest();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendRegister( const FString& InID, const FString& InEmail, const TArray<uint8>& HashedPassword ) const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateInventory( const int32 Page ) const;
	
	UFUNCTION(Client, Reliable)
	void Client_PropagateRegisterStatus(bool bSuccess, const ERegisterFailCodeUE FailCode);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_Login, meta=(AllowPrivateAccess=true))
	bool bLogin;
	
	UPROPERTY(VisibleAnywhere, Replicated, meta=(AllowPrivateAccess=true))
	uint32 UserID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta=(AllowPrivateAccess=true))
	FUUIDWrapper SessionID;
	
};
