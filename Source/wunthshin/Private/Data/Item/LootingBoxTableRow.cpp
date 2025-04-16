// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/LootingBoxTableRow.h"

inline void FLootingBoxTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	ItemType = EItemType::LootingBox;

	Super::OnDataTableChanged(InDataTable, InRowName);
}
