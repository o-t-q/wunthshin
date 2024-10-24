// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemMetadataGetter.generated.h"

class USG_WSItemMetadata;
// This class does not need to be modified.
UINTERFACE()
class UItemMetadataGetter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WUNTHSHIN_API IItemMetadataGetter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual USG_WSItemMetadata* GetMetadata(const FName& InAssetName) = 0;
};
