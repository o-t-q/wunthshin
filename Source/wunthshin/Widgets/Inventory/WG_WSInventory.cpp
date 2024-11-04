// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSInventory.h"

#include "GeometryCollection/ManagedArrayTypes.h"
#include "Kismet/GameplayStatics.h"
#include "wunthshin/Actors/AA_WSCharacter.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"

void UWG_WSInventory::NativeConstruct()
{
	Super::NativeConstruct();

	// 인벤토리 컴포넌트의 TArray를 복사
	AA_WSCharacter* Player = Cast<AA_WSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	TArray<FInventoryPair> Origin;
	TArray<UInventoryEntryData*> NewArray;
	auto& Items = Player->GetComponentByClass<UC_WSInventory>()->GetItems();

	for (auto& Item : Items)
	{
		NewArray.Emplace(NewObject<UInventoryEntryData>(this));
	}
	
	TileView->SetListItems(NewArray);
}
