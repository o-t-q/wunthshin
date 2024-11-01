#pragma once

#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"

#include "NPCTableRow.generated.h"

class UBehaviorTree;

USTRUCT()
struct WUNTHSHIN_API FNPCTableRow : public FCharacterTableRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "AI")
	bool bUseAI;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bUseAI", EditConditionHide))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere)
	bool bEnemy;

};