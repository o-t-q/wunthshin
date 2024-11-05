// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "CharacterSubsystem.generated.h"

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
	
public:
	UCharacterSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};