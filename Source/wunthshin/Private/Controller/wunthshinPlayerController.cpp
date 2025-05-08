// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/wunthshinPlayerController.h"

#include "wunthshinPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Network/Subsystem/WSServerSubsystem.h"

AwunthshinPlayerController::AwunthshinPlayerController(): bLogin(false), UserID(0)
{
	bEnableStreamingSource = true;
	bStreamingSourceShouldActivate = true;
}

void AwunthshinPlayerController::GetLifetimeReplicatedProps( TArray<class FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );
	DOREPLIFETIME_CONDITION( AwunthshinPlayerController, bLogin, COND_OwnerOnly )
	DOREPLIFETIME_CONDITION( AwunthshinPlayerController, UserID, COND_OwnerOnly )
	DOREPLIFETIME_CONDITION( AwunthshinPlayerController, SessionID, COND_OwnerOnly )
}

void AwunthshinPlayerController::SetUserID( const uint32 InUserID )
{
	UserID = InUserID;
	if (AwunthshinPlayerState* Casted = GetPlayerState<AwunthshinPlayerState>())
	{
		Casted->UserID = InUserID;
	}
}

void AwunthshinPlayerController::OnRep_Login() const
{
	OnLoginStatusChanged.Broadcast();
}

void AwunthshinPlayerController::Client_AuthenticateAndMainMap_Implementation()
{
	ClientTravel(TEXT("/Game/Level/AllLevel"), TRAVEL_Absolute);
}

bool AwunthshinPlayerController::Server_Authenticate_Validate(const int32 InUserID, const FUUIDWrapper& InSessionID)
{
	return true;
}

void AwunthshinPlayerController::Server_UpdateInventory_Implementation( const int32 Page ) const
{
	UWSServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
	check(ServerSubsystem);

	if ( ServerSubsystem )
	{
		ServerSubsystem->Server_GetItems( this, Page );
	}
}

bool AwunthshinPlayerController::Server_UpdateInventory_Validate( const int32 Page )
{
	// todo: rate limit
	return true;
}

void AwunthshinPlayerController::Server_Authenticate_Implementation(const int32 InUserID, const FUUIDWrapper& InSessionID)
{
	UserID = InUserID;
	SessionID = InSessionID;
	bLogin = true;
	
	Client_AuthenticateAndMainMap();
}

void AwunthshinPlayerController::Server_SendLogoutRequest_Implementation( )
{
	UWSServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
	check(ServerSubsystem);

	if ( ServerSubsystem )
	{
		check( ServerSubsystem->Server_SendLogoutRequest( this ) );
	}
}

bool AwunthshinPlayerController::Server_SendLogoutRequest_Validate( )
{
	return UWSServerSubsystem::ValidateLogoutRequest( this );
}
