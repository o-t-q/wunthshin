#pragma once

#include "CharacterTableRow.generated.h"

USTRUCT()
struct WUNTHSHIN_API FCharacterTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName CharacterName;

	UPROPERTY(EditAnywhere)
	UTexture2D* CharacterIcon;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAnimInstance> AnimInstance;

	UPROPERTY(EditAnywhere, Category = "Collision")
	bool bCustomCapsuleSize;

	UPROPERTY(EditAnywhere, Category = "Collision", meta = (EditCondition = "bCustomCapsuleSize", EditConditionHide))
	float Radius;

	UPROPERTY(EditAnywhere, Category = "Collision", meta = (EditCondition = "bCustomCapsuleSize", EditConditionHide))
	float HalfHeight;

	UPROPERTY(EditAnywhere, meta=(RowType = "/Script/wunthshin.CharacterStats"))
	FDataTableRowHandle Stats;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> HitMontages;
};