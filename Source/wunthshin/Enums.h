#pragma once

#include "Enums.generated.h"

UENUM()
enum class EItemType : uint8 
{
	Consumable,
	Weapon
};

UENUM()
enum class EWeaponType : uint8
{
	Melee,
	Range
};