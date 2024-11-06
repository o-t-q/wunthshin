// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSInventory.h"

#include "WG_WSInventoryEntry.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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

	AA_WSCharacter* Player = Cast<AA_WSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	PlayerInventory = Player->GetComponentByClass<UC_WSInventory>();
	
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
	TArray<UInventoryEntryData*> NewArray;
	auto& Items = PlayerInventory->GetItems();

	for (auto& Item : Items)
	{
		auto newItem = NewObject<UInventoryEntryData>(this);
		newItem->Initialize(Item);
		NewArray.Emplace(newItem);
	}
	
	TileView->SetListItems(NewArray);

	//
	uint32 InvenCount = Items.Num();
	uint32 InvenMaxCount = 1000; // 임의로 정함
	
	FString InText = FString::Printf(TEXT("%i/%i"), InvenCount,InvenMaxCount);
	InventoryCount->SetText(FText::FromString(InText));

	Button_ClosePanel->OnClicked.AddDynamic(this, &ThisClass::OnClickButton_ClosePanel);

	// 선택된 아이템 정보 출력
	auto Selected = TileView->GetSelectedItem<UWG_WSInventoryEntry>();
	if(!Selected) return;
	
	auto AssetName = Selected->GetData()->EntryData.Metadata->GetAssetName(); 
	ItemName->SetText(FText::FromName(AssetName));
	
	auto itemIcon = Selected->GetData()->EntryData.Metadata->GetItemIcon();
	ItemIcon->SetBrushFromTexture(itemIcon);

	//auto itemType = Selected->GetData()->EntryData.Metadata->GetItemType();

	auto itemCount = Selected->GetData()->EntryData.Count;
	ItemCount->SetText(FText::FromString(FString::Printf(TEXT("%i"), itemCount)));

	//auto itemEfficiency = Selected->GetData()->EntryData.Metadata->

	auto itemDesc = Selected->GetData()->EntryData.Metadata->GetItemDescription();
	ItemDescription->SetText(FText::FromName(itemDesc));
	
	auto selected = TileView->GetSelectedItem<UWG_WSInventoryEntry>();
}

void UWG_WSInventory::OnClickButton_ClosePanel()
{
	ESlateVisibility bIsVisible = IsVisible() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	SetVisibility(bIsVisible);
}




