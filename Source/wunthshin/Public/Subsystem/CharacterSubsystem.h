// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interface/DataTableQuery.h"
#include "CharacterSubsystem.generated.h"


class AwunthshinPlayerController;
class AClientCharacterInfo;

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UCharacterSubsystem : public UGameInstanceSubsystem, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* AssetDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* StatDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table", meta=(AllowPrivateAccess = "true"))
	TMap<int32, AClientCharacterInfo*> ClientCharacters;
	
public:
	UCharacterSubsystem();

	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;

	AClientCharacterInfo* InitializeClientInfo( APlayerController* Controller, const int32 InUserID );

	AClientCharacterInfo* GetClientInfo( const int32 InUserID ) const;

	AClientCharacterInfo* GetClientInfo( const AwunthshinPlayerController* PlayerController ) const;

	AClientCharacterInfo* GetFirstPlayerControllerCharacterInfo() const;

	void DestroyClientInfo( const APlayerController* Controller );
};