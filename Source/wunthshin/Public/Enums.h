#pragma once
#include "message.h"

#include "Enums.generated.h"

UENUM()
enum class EItemType : uint8 
{
	Unknown = EDBItemType::Unknown,
	Consumable = EDBItemType::Consumable,
	Weapon = EDBItemType::Weapon,
	Max = EDBItemType::MAX
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
