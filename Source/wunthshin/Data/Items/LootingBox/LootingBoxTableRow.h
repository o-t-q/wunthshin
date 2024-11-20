// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LootingBoxTableRow.generated.h"

/***************************************************************************************************
 *
 *		FLootingBoxTableRow
 *
 *		- 아이템의 타입, 레어리티, 최대 수량, 최소 수량만 정의한다
 */

struct FItemTableRow;
enum class ERarity : uint8;
enum class EItemType : uint8;

USTRUCT()
struct WUNTHSHIN_API FLootItem
{
	GENERATED_BODY()

public:
	// // 아이템테이블에 정의된 아이템만 사용한다
	// UPROPERTY(EditAnywhere)
	// UDataTable* ItemDataTable;

	// // 필수아이템 여부
	// UPROPERTY(EditAnywhere)
	// bool bIsEssential = false;

	// 필수아이템은 AssetName으로 ItemTable에서 정보를 가져온다, 랜덤아이템이면 무시한다
	UPROPERTY(EditAnywhere)
	FDataTableRowHandle ItemRowHandle;
	
	// 랜덤아이템 타입
	UPROPERTY(EditAnywhere)
	EItemType ItemType;

	// 랜덤아이템 등급
	UPROPERTY(EditAnywhere)
	ERarity ItemRarity;
	
	// 뜨게할 최소수량, 최대수량(루팅박스 초기화 시 최소최대 사이의 랜덤수량으로 초기화한다)
	UPROPERTY(EditAnywhere)
	uint64 MinQuantity = 1;

	UPROPERTY(EditAnywhere)
	uint64 MaxQuantity;
};


USTRUCT()
struct WUNTHSHIN_API FLootingBoxTableRow : public FTableRowBase
{
	friend FLootItem;
	
	GENERATED_BODY()

public:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
	{
		// MaxDiversity만큼 RandomItems 크기를 강제
		
	}
	
public:
	// 랜덤아이템 최대 종류 수(MinDiversity ~ Diversity 사이 랜덤)
	UPROPERTY(EditAnywhere)
	uint64 MinDiversity = 1;
	
	UPROPERTY(EditAnywhere)
	uint64 MaxDiversity;

	// 랜덤으로 뜨게할 아이템, 종류 수 만큼만 생성된다
	// 아이템테이블에 랜덤아이템(등급, 타입, 지역(예정))를 정의하여 해당 배열에 할당한다
	// 또는 FLootItem 자체에서 정의
	UPROPERTY(EditAnywhere)
	TArray<FLootItem> RandomItems;
	
	// 필수로 뜨게할 아이템, 종류 수 영향 X
	UPROPERTY(EditAnywhere)
	TArray<FLootItem> EssentialItems;

};
