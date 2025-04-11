// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainGame/WG_WSItemLogEntry.h"

#include "Actor/Item/A_LootingBox.h"

void UWG_WSItemLogEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	auto Data = Cast<AA_LootingBox>(ListItemObject);
	
}

FReply UWG_WSItemLogEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
}
