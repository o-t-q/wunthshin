// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/wunthshinPlayerController.h"

#include "wunthshinGameMode.h"
#include "wunthshinPlayerState.h"
#include "Actor/Pawn/AA_WSCharacter.h"

#include "Data/Character/ClientCharacterInfo.h"
#include "Net/UnrealNetwork.h"
#include "Network/Channel/WSLoginChannel.h"
#include "Network/Subsystem/WSServerSubsystem.h"

#include "Subsystem/CharacterSubsystem.h"

constexpr static size_t IDSizeLimit = sizeof(decltype(std::declval<LoginMessage>().name._Elems));

AwunthshinPlayerController::AwunthshinPlayerController()
{
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

void AwunthshinPlayerController::OnRep_Login() const
{
	OnLoginStatusChanged.Broadcast();
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
		check( ServerSubsystem->Server_SendLoginRequest( this, InID, HashedPasswordWrapper ) );
	}
}

bool AwunthshinPlayerController::Server_SendLoginRequest_Validate( const FString& InID,
	const TArray<uint8>& HashedPassword )
{
	return UWSServerSubsystem::ValidateLoginRequest( InID, HashedPassword );
}
