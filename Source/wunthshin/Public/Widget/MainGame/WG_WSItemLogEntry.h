// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/WG_WSUserWidgetBase.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "WG_WSItemLogEntry.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSItemLogEntry : public UWG_WSUserWidgetBase, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

};
