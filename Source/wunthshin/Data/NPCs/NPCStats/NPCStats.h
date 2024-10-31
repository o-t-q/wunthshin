#pragma once

#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"

#include "NPCStats.generated.h"

// NPC의 스탯을 정의하는 구조체
USTRUCT(BlueprintType)
struct FNPCStats : public FCharacterStats
{
    GENERATED_BODY()

    // 추가적인 스탯을 여기서 정의할 수 있습니다.
};