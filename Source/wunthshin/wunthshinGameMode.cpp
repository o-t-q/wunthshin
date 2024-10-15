// Copyright Epic Games, Inc. All Rights Reserved.

#include "wunthshinGameMode.h"
#include "A_WSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AwunthshinGameMode::AwunthshinGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
