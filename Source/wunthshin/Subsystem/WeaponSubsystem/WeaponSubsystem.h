// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "wunthshin/Interfaces/ItemMetadataGetter/ItemMetadataGetter.h"
#include "wunthshin/Subsystem/TableQuerySubsystem.h"

#include "WeaponSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWeaponSubsystem : public UTableQueryGameInstanceSubsystem, public IItemMetadataGetter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	TMap<FName, USG_WSItemMetadata*> Metadata;
	
public:
	UWeaponSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual USG_WSItemMetadata* GetMetadata(const FName& InAssetName) override;
};

UCLASS()
class WUNTHSHIN_API UWeaponEditorSubsystem : public UTableQueryEditorSubsystem, public IItemMetadataGetter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta=(AllowPrivateAccess = "true"))
	TMap<FName, USG_WSItemMetadata*> Metadata;
	
public:
	UWeaponEditorSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual USG_WSItemMetadata* GetMetadata(const FName& InAssetName) override;

};
