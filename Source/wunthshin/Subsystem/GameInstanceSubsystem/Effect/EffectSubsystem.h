// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wunthshin/Data/Effects/EffectRowHandle/EffectRowHandle.h"

#include "wunthshin/Data/Effects/EffectTableRow/EffectTableRow.h"
#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "EffectSubsystem.generated.h"

class UO_WSBaseEffect;

DECLARE_LOG_CATEGORY_EXTERN(LogEffectSubsystem, Log, All);

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UEffectSubsystem : public UGameInstanceSubsystem, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Effector", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<UO_WSBaseEffect>, UO_WSBaseEffect*> PreinstantiatedEffectors;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* DataTable;

public:
	UEffectSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UO_WSBaseEffect* GetEffector(const FEffectRowHandle& InRowHandle) const;

	UO_WSBaseEffect* GetEffector(TSubclassOf<UO_WSBaseEffect> InReactorType) const;
};
