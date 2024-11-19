#pragma once

#include "Enums.generated.h"

UENUM()
enum class EItemType : uint8 
{
	Consumable,
	Weapon,
	Max,
};

UENUM()
enum class EWeaponType : uint8
{
	Melee,
	Range
};

// 일단 아이템 등급으로 사용
UENUM()
enum class ERarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Unique,
	Legendary
};