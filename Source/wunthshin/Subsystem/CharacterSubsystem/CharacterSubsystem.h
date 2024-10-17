// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "wunthshin/Subsystem/TableQueryEditorSubsystem.h"
#include "wunthshin/Subsystem/TableQueryGameInstanceSubsystem.h"
#include "CharacterSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UCharacterSubsystem : public UTableQueryGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UCharacterSubsystem();

};

UCLASS()
class WUNTHSHIN_API UCharacterEditorSubsystem : public UTableQueryEditorSubsystem
{
	GENERATED_BODY()
	
public:
	UCharacterEditorSubsystem();

};
