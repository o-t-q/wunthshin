#pragma once

#include "Data/Character/CharacterTableRow.h"

#include "NPCTableRow.generated.h"

class UBehaviorTree;
class UBlueprintAIPerceptionComponent;

USTRUCT()
struct WUNTHSHIN_API FNPCTableRow : public FCharacterTableRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "AI")
	bool bUseAI;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bUseAI", EditConditionHide))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bUseAI", EditConditionHide))
	TSubclassOf<UBlueprintAIPerceptionComponent> UserDefinedSensory;
	
	UPROPERTY(EditAnywhere)
	bool bEnemy;
	
};