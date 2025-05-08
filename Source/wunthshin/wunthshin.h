// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "message.h"
constexpr static size_t IDSizeLimit = sizeof(decltype(std::declval<LoginMessage>().name._Elems));