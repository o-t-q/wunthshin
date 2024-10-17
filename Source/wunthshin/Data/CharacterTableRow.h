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
};