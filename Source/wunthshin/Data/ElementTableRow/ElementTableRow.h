#pragma once

#include "CoreMinimal.h"
#include "ElementTableRow.generated.h"

class UO_WSElementReactor;

USTRUCT(BlueprintType)
struct WUNTHSHIN_API FElementReaction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(RowType="ElementTableRow"))
	FDataTableRowHandle OtherElement;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UO_WSElementReactor> Reactor;

	bool operator==(const FElementReaction& Other) const;
};

USTRUCT()
struct WUNTHSHIN_API FElementTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FName ElementName;

	UPROPERTY(EditDefaultsOnly)
	TArray<FElementReaction> Reactions;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName);
};