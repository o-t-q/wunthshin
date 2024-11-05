// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSInventory.h"

#include "WG_WSInventoryEntry.h"
#include "GeometryCollection/ManagedArrayTypes.h"
#include "Kismet/GameplayStatics.h"
#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"


void UWG_WSInventory::NativeConstruct()
{
	Super::NativeConstruct();
	
	// static ConstructorHelpers::FObjectFinder<UTexture2D> RarityBGCommon(TEXT("/Script/Engine.Texture2D'/Game/Sprites/UI/ItemRarity/T_QualityGrey.T_QualityGrey'"));
	// check(RarityBGCommon.Object)
	// RarityBackground.Emplace(ERarity::Common, NewObject<UTexture2D>(RarityBGCommon.Object));
	//
	// static ConstructorHelpers::FObjectFinder<UTexture2D> RarityBGUnCommon(TEXT("/Script/Engine.Texture2D'/Game/Sprites/UI/ItemRarity/T_QualityGreen.T_QualityGreen'"));
	// check(RarityBGUnCommon.Object)
	// RarityBackground.Emplace(ERarity::Uncommon, NewObject<UTexture2D>(RarityBGUnCommon.Object));
	//
	// static ConstructorHelpers::FObjectFinder<UTexture2D> RarityBGRare(TEXT("/Script/Engine.Texture2D'/Game/Sprites/UI/ItemRarity/T_QualityBlue.T_QualityBlue'"));
	// check(RarityBGRare.Object)
	// RarityBackground.Emplace(ERarity::Rare, NewObject<UTexture2D>(RarityBGRare.Object));
	//
	// static ConstructorHelpers::FObjectFinder<UTexture2D> RarityBGUnique(TEXT("/Script/Engine.Texture2D'/Game/Sprites/UI/ItemRarity/T_QualityPurple.T_QualityPurple'"));
	// check(RarityBGUnique.Object)
	// RarityBackground.Emplace(ERarity::Unique, NewObject<UTexture2D>(RarityBGUnique.Object));
	//
	// static ConstructorHelpers::FObjectFinder<UTexture2D> RarityBGLegend(TEXT("/Script/Engine.Texture2D'/Game/Sprites/UI/ItemRarity/T_QualityGold.T_QualityGold'"));
	// check(RarityBGLegend.Object)
	// RarityBackground.Emplace(ERarity::Legendary, NewObject<UTexture2D>(RarityBGLegend.Object));

	
	// todo: 아이템 획득하는 delegate에 RefreshListItem() 바인딩
	
	RefreshListItem();
}

void UWG_WSInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Test
	if (UGameplayStatics::GetPlayerController(GetWorld(),0)->WasInputKeyJustPressed(FKey(TEXT("Q"))))
	{
		RefreshListItem();
	}
}

void UWG_WSInventory::RefreshListItem()
{
	// 인벤토리 컴포넌트의 TArray를 복사
	AA_WSCharacter* Player = Cast<AA_WSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	TArray<UInventoryEntryData*> NewArray;
	auto& Items = Player->GetComponentByClass<UC_WSInventory>()->GetItems();

	for (auto& Item : Items)
	{
		auto newItem = NewObject<UInventoryEntryData>(this);
		newItem->Initialize(Item);
		NewArray.Emplace(newItem);
	}
	
	TileView->SetListItems(NewArray);
}
