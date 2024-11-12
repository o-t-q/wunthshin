// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WSSerializeClass.generated.h"

struct FWSArchive;
// This class does not need to be modified.
UINTERFACE()
class UWSSerializeClass : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WUNTHSHIN_API IWSSerializeClass
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Serialize(FWSArchive& Ar) = 0;
};
