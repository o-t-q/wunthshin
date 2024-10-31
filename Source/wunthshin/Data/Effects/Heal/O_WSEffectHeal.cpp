// Fill out your copyright notice in the Description page of Project Settings.


#include "wunthshin/Data/Effects/Heal/O_WSEffectHeal.h"

#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Components/Stats/StatsComponent.h"

DEFINE_LOG_CATEGORY(LogItemEffectHeal);

void UO_WSEffectHeal::Effect(const FEffectParameter& InParameters, AActor* InInstigator, AActor* InTargetActor) const
{
	if (InParameters.FloatValues.Contains("Heal"))
	{
		float Value = InParameters.FloatValues["Heal"];
		UE_LOG(LogItemEffectHeal, Log, TEXT("Healing %s with %f amount"), *InTargetActor->GetName(), Value);
		InTargetActor->GetComponentByClass<UStatsComponent>()->IncreaseHP(Value);
	}
	else 
	{
		UE_LOG(LogItemEffectHeal, Error, TEXT("Heal value not found!"));
	}
}
