// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/SG_WSItemMetadata.h"
#include "Data/Effect/EffectTableRow.h"
#include "Subsystem/EffectSubsystem.h"

const UO_WSBaseEffect* USG_WSItemMetadata::GetItemEffect(const UWorld* InWorld) const
{
    return InWorld->GetGameInstance()->GetSubsystem<UEffectSubsystem>()->GetEffector(ItemEffect);
}

const FEffectParameter& USG_WSItemMetadata::GetItemParameter() const
{
    return ItemParameter;
}
