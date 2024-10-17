// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "TableQueryEditorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UTableQueryEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table")
	UDataTable* DataTable;

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	FDataTableRowHandle FindItem(const FName& InKey) const;
};
