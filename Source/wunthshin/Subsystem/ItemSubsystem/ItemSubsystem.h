// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "wunthshin/Subsystem/TableQueryGameInstanceSubsystem.h"
#include "ItemSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UItemSubsystem : public UTableQueryGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UItemSubsystem();

};
