#pragma once

#include "CharacterStats.generated.h"

// 캐릭터의 스탯을 정의하는 구조체
USTRUCT(BlueprintType)
struct FCharacterStats : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Stats")
    float HP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
    float MaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
    float Stamina;

    // 추가적인 스탯을 여기서 정의할 수 있습니다.
};