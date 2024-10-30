// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "wunthshin/Data/Effects/EffectTableRow/EffectTableRow.h"

#include "SG_WSItemMetadata.generated.h"

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

	UPROPERTY()
	EItemType ItemType;

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
	
	FName GetAssetName() const { return AssetName; }

	template <EItemType ItemT>
	bool IsItem() const
	{
		return ItemT == ItemType;
	}
	
};
