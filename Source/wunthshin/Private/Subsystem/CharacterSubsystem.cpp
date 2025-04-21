// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CharacterSubsystem.h"

#include "wunthshinPlayerState.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/Pawn/AA_WSCharacter.h"
#include "Component/StatsComponent.h"

#include "Controller/wunthshinPlayerController.h"

#include "Data/Character/SG_WSCharacterSnapshot.h"
#include "Data/Character/CharacterStats.h"
#include "Data/Character/CharacterTableRow.h"
#include "Data/Character/ClientCharacterInfo.h"

UCharacterSubsystem::UCharacterSubsystem()
	: AssetDataTable(nullptr), StatDataTable(nullptr) {}

void UCharacterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AssetDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterTable.DT_CharacterTable'")));
	check(AssetDataTable);
	StatDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_CharacterStatsTable.DT_CharacterStatsTable'")));
	check(StatDataTable);
	
	DataTableMapping.Emplace(FCharacterTableRow::StaticStruct(), AssetDataTable);
	DataTableMapping.Emplace(FCharacterStats::StaticStruct(), StatDataTable);
}

AClientCharacterInfo* UCharacterSubsystem::InitializeClientInfo( APlayerController* Controller, const int32 InUserID )
{
	if ( GetWorld()->GetNetMode() == NM_Client )
	{
		check(false); // Client does not have valid data
		return nullptr;
	}

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = Controller;
	ActorSpawnParameters.ObjectFlags = RF_Transient;
	ActorSpawnParameters.bNoFail = true;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AClientCharacterInfo* Info = GetWorld()->SpawnActor<AClientCharacterInfo>( FVector::ZeroVector, FRotator::ZeroRotator, ActorSpawnParameters );

	check( Info ); // Info spawn failed
	ClientCharacters.Emplace( InUserID, Info );

	// todo: Sync character with database

	// Set the user client character context to player state, so that the client can reference.
	Controller->GetPlayerState<AwunthshinPlayerState>()->CachedClientCharacter = Info;
	return Info;
}

AClientCharacterInfo* UCharacterSubsystem::GetClientInfo( const int32 InUserID ) const
{
	if ( GetWorld()->GetNetMode() == NM_Client )
	{
		check(false); // Client does not have valid data
		return nullptr;
	}

	if ( ClientCharacters.Contains( InUserID ) )
	{
		return ClientCharacters[InUserID];
	}

	check(false); // Unknown Client ID
	return nullptr;
}

AClientCharacterInfo* UCharacterSubsystem::GetClientInfo(const AwunthshinPlayerController* PlayerController) const
{
	if ( GetWorld()->GetNetMode() == NM_Client )
	{
		check(false); // Client does not have valid data
		return nullptr;
	}

	const int32 UserID = PlayerController->GetPlayerState<AwunthshinPlayerState>()->GetUserID();

	if ( UserID == -1 )
	{
		return nullptr;
	}

	return GetClientInfo( UserID );
}

AClientCharacterInfo* UCharacterSubsystem::GetFirstPlayerControllerCharacterInfo() const
{
	return GetClientInfo( GetWorld()->GetFirstPlayerController<AwunthshinPlayerController>() );
}

void UCharacterSubsystem::DestroyClientInfo( const APlayerController* Controller )
{
	if ( const AwunthshinPlayerState* PlayerState = Controller->GetPlayerState<AwunthshinPlayerState>() )
	{
		ClientCharacters.Remove( PlayerState->GetUserID() );
	}
}

