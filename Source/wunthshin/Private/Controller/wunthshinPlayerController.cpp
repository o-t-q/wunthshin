// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/wunthshinPlayerController.h"

#include "wunthshinPlayerState.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"
#include "Network/Subsystem/WSServerSubsystem.h"

AwunthshinPlayerController::AwunthshinPlayerController()
{
	bEnableStreamingSource = true;
	bStreamingSourceShouldActivate = true;
}

void AwunthshinPlayerController::Client_PropagateRegisterStatus_Implementation( bool bSuccess,
	const ERegisterFailCodeUE FailCode )
{
	if ( GetWorld()->GetFirstPlayerController() == this )
	{
		LastRegisterationStatus.Broadcast( bSuccess, FailCode );
	}
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

void AwunthshinPlayerController::Client_OpenStartLevel_Implementation() const
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Level/Start_Map"));
}

void AwunthshinPlayerController::Server_MoveToStart_Implementation() const
{
	Client_OpenStartLevel();
}

bool AwunthshinPlayerController::Server_MoveToStart_Validate()
{
	return !bLogin;
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

void AwunthshinPlayerController::Server_SendRegister_Implementation( const FString& InID, const FString& InEmail,
                                                                     const TArray<uint8>& HashedPassword ) const
{
	UWSServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
	check(ServerSubsystem);

	if ( ServerSubsystem )
	{
		FSHA256Signature HashedPasswordWrapper{};
		for (size_t i = 0; i < HashedPassword.Num(); ++i)
		{
			HashedPasswordWrapper.Signature[i] = HashedPassword[i];
		}
		check(ServerSubsystem->Server_SendRegister( this, InID, InEmail, HashedPasswordWrapper ));
	}
	
}

bool AwunthshinPlayerController::Server_SendRegister_Validate( const FString& InID, const FString& InEmail,
	const TArray<uint8>& HashedPassword )
{
	UWSServerSubsystem::ValidateRegisterRequest( InID, InEmail, HashedPassword );
	return true;
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

void AwunthshinPlayerController::Server_SendLoginRequest_Implementation( const FString& InID,
                                                                         const TArray<uint8>& HashedPassword )
{
	UWSServerSubsystem* ServerSubsystem = GetGameInstance()->GetSubsystem<UWSServerSubsystem>();
	check(ServerSubsystem);

	if ( ServerSubsystem )
	{
		FSHA256Signature HashedPasswordWrapper{};
		for (size_t i = 0; i < HashedPassword.Num(); ++i)
		{
			HashedPasswordWrapper.Signature[i] = HashedPassword[i];
		}
		const bool Result = ServerSubsystem->Server_SendLoginRequest( this, InID, HashedPasswordWrapper );
		check( Result );
	}
}

bool AwunthshinPlayerController::Server_SendLoginRequest_Validate( const FString& InID,
	const TArray<uint8>& HashedPassword )
{
	return UWSServerSubsystem::ValidateLoginRequest( InID, HashedPassword );
}
