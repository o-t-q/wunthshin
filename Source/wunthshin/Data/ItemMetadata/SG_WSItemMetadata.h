// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SG_WSItemMetadata.generated.h"

enum class EItemType : uint8;

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API USG_WSItemMetadata : public USaveGame
{
	GENERATED_BODY()

	friend struct FItemTableRow;

	UPROPERTY()
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere)
	FName AssetName;

public:
	FName GetAssetName() const { return AssetName; }

	template <EItemType ItemT>
	bool IsItem() const
	{
		return ItemT == ItemType;
	}
	
};
