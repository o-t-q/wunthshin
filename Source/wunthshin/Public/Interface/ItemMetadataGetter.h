// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemMetadataGetter.generated.h"

enum class EItemType : uint8_t;
class USG_WSItemMetadata;

USTRUCT(BlueprintType)
struct FWSMetadataPair
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<FName, USG_WSItemMetadata*> Metadata;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TArray<USG_WSItemMetadata*> MetadataByID;
};

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

	virtual USG_WSItemMetadata* GetMetadata(const EItemType InItemType, const FName& InAssetName) = 0;
	virtual USG_WSItemMetadata* GetMetadata(const EItemType InItemType, const int32 InID) = 0;
};
