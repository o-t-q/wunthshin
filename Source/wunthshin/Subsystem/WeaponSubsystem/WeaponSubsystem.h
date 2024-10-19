// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "wunthshin/Subsystem/TableQuerySubsystem.h"
#include "WeaponSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWeaponSubsystem : public UTableQueryGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UWeaponSubsystem();

};

UCLASS()
class WUNTHSHIN_API UWeaponEditorSubsystem : public UTableQueryEditorSubsystem
{
	GENERATED_BODY()
	
public:
	UWeaponEditorSubsystem();

};
