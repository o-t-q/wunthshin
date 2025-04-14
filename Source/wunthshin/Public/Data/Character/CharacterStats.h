#pragma once

#include "CharacterStats.generated.h"

struct FWSArchive;

USTRUCT(BlueprintType)
struct WUNTHSHIN_API FCharacterMovementModifier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WalkMaxSpeed = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NormalMaxSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FastMaxSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CrouchMaxSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InitialJumpVelocity = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxFlyingSpeed = 1.f;
};

USTRUCT(BlueprintType)
struct WUNTHSHIN_API FCharacterMovementStats
{
	GENERATED_BODY()
	virtual ~FCharacterMovementStats() = default;
	
	float GetWalkSpeed() const { return WalkMaxSpeed * MovementModifier.WalkMaxSpeed; }
	float GetNormalMaxSpeed() const { return NormalMaxSpeed * MovementModifier.NormalMaxSpeed; }
	float GetFastMaxSpeed() const { return FastMaxSpeed * MovementModifier.FastMaxSpeed; }
	float GetCrouchMaxSpeed() const { return CrouchMaxSpeed * MovementModifier.CrouchMaxSpeed; }
	float GetInitialJumpVelocity() const { return InitialJumpVelocity * MovementModifier.InitialJumpVelocity; }
	float GetFlyingMaxSpeed() const { return FlyingMaxSpeed * MovementModifier.MaxFlyingSpeed; }

	void SetModifier(const FCharacterMovementModifier& Modifier) { MovementModifier = Modifier; }
	void ResetModifier() { MovementModifier = {}; }

private:
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
	float FlyingMaxSpeed = 1000.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Modifier", meta = (AllowPrivateAccess = "true"))
	FCharacterMovementModifier MovementModifier;
};

// 캐릭터의 스탯을 정의하는 구조체
USTRUCT(BlueprintType)
struct WUNTHSHIN_API FCharacterStats : public FTableRowBase
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
};