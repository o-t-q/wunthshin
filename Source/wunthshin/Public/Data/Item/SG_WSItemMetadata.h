// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/Effect/EffectParameter.h"
#include "Data/Effect/EffectRowHandle.h"

#include "SG_WSItemMetadata.generated.h"

enum class ERarity : uint8;
class UO_WSBaseEffect;
enum class EItemType : uint8;

/**
 * 런타임에서 아이템의 간략한 정보를 담는 객체
 */
UCLASS()
class WUNTHSHIN_API USG_WSItemMetadata : public USaveGame
{
	GENERATED_BODY()

	friend struct FItemSubsystemUtility;

private:
	UPROPERTY()
	ERarity ItemRarity;

	UPROPERTY()
	EItemType ItemType;
	
	UPROPERTY(VisibleAnywhere)
	int32 ID;

	UPROPERTY(VisibleAnywhere)
	FName AssetName;

	UPROPERTY(VisibleAnywhere)
	UTexture2D* ItemIcon;

	UPROPERTY(VisibleAnywhere)
	FName ItemDescription;

	UPROPERTY(VisibleAnywhere)
	FEffectRowHandle ItemEffect;

	UPROPERTY(VisibleAnywhere)
	FEffectParameter ItemParameter;

public:
	// 아이템의 효과 처리기 getter, Metadata가 Transient으로 생성된 경우 World가 필요함
	const UO_WSBaseEffect* GetItemEffect(const UWorld* InWorld) const;

	const FEffectParameter& GetItemParameter() const;
	
	int32 GetID() const { return ID; }
	FName GetAssetName() const { return AssetName; }
	UTexture2D* GetItemIcon() const { return ItemIcon; }
	FName GetItemDescription() const { return ItemDescription; }
	ERarity GetItemRarity() const { return ItemRarity; }
	EItemType GetItemType() const { return ItemType; }
	
	template <EItemType ItemT>
	bool IsItem() const
	{
		return ItemT == ItemType;
	}
	
};
