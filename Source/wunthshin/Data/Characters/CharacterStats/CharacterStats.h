#pragma once
#include "wunthshin/Data/SerializeStruct/WSSerializeStruct.h"

#include "CharacterStats.generated.h"

struct FWSArchive;

USTRUCT(BlueprintType)
struct FCharacterMovementStats : public FWSSerializeStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = "0"))
	float WalkMaxSpeed = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = "0"))
	float NormalMaxSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = "0"))
	float FastMaxSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = 0))
	float CrouchMaxSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = 0))
	float InitialJumpVelocity = 700.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = 0))
	float MaxFlyingSpeed = 1000.f;

	virtual void Serialize(FWSArchive& Ar) override
	{
		Ar << WalkMaxSpeed;
		Ar << NormalMaxSpeed;
		Ar << FastMaxSpeed;
		Ar << CrouchMaxSpeed;
		Ar << InitialJumpVelocity;
		Ar << MaxFlyingSpeed;
	}
};

// 캐릭터의 스탯을 정의하는 구조체
USTRUCT(BlueprintType)
struct FCharacterStats : public FTableRowBase, public FWSSerializeStruct
{
    GENERATED_BODY()
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Stats", meta=(ClampMin = 0))
    float HP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta=(ClampMin = 0))
    float MaxHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats", meta=(ClampMin = 0))
    float Stamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats")
	FCharacterMovementStats Movement;
    
    // 추가적인 스탯을 여기서 정의할 수 있습니다.

	virtual void Serialize(FWSArchive& Ar) override
	{
		Ar << HP;
		Ar << Stamina;
		Ar << Movement;
	}
};