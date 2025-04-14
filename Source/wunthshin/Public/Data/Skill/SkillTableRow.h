#pragma once
#include "LevelSequence.h"

#include "SkillTableRow.generated.h"

class ULevelSequence;
class UO_WSBaseSkill;

// 스킬에 매개변수로 주어질 값
USTRUCT()
struct WUNTHSHIN_API FSkillParameter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ULevelSequence* CastingSequence; 
	
	UPROPERTY(EditAnywhere)
	float StaminaCost;

	UPROPERTY(EditAnywhere)
	float CastingDuration = 0.f;

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

USTRUCT()
struct WUNTHSHIN_API FSkillTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName SkillName;

	UPROPERTY(EditAnywhere)
	FName SkillDescription;

	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/wunthshin.ElementTableRow"))
	FDataTableRowHandle Element;
	
	// 타겟이 필요한 경우
	UPROPERTY(EditAnywhere)
	bool bTargeting;
	
	UPROPERTY(EditAnywhere)
	FSkillParameter Parameter;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UO_WSBaseSkill> SkillProcessor;
	
};
