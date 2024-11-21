// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSItemLogEntry.h"

void UWG_WSItemLogEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
}

FReply UWG_WSItemLogEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
}
