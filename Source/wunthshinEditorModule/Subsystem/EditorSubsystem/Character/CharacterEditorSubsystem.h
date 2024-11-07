// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "CharacterEditorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHINEDITORMODULE_API UCharacterEditorSubsystem : public UEditorSubsystem, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* AssetDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* StatDataTable;
	
public:
	UCharacterEditorSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool IsEditorOnly() const override { return true; }

};
