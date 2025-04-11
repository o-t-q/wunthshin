#pragma once

#include "EffectParameter.generated.h"

// 효과에 매개변수로 주어질 값
USTRUCT()
struct WUNTHSHIN_API FEffectParameter
{
	GENERATED_BODY()

	// 도트형일 경우
	UPROPERTY(EditAnywhere)
	float Duration = 0.f;

	// 기준 시간 (초 단위)
	UPROPERTY(EditAnywhere)
	float PerTime = 1.f;

	// float형 값
	UPROPERTY(EditAnywhere)
	TMap<FName, float> FloatValues;

	// int형 값
	UPROPERTY(EditAnywhere)
	TMap<FName, int> IntegerValues;

	// Vector형 값
	UPROPERTY(EditAnywhere)
	TMap<FName, FVector> VectorValues;

};