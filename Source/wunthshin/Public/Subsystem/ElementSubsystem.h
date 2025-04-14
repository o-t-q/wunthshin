// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/Element/ElementRowHandle.h"
#include "Interface/DataTableQuery.h"
#include "ElementSubsystem.generated.h"

class UO_WSElementReactor;

DECLARE_LOG_CATEGORY_EXTERN(LogElementSubsystem, Log, All);

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UElementSubsystem : public UGameInstanceSubsystem, public IDataTableQuery
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Reactor", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<UO_WSElementReactor>, UO_WSElementReactor*> PreinstantiatedReactors;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Table", meta=(AllowPrivateAccess = "true"))
	UDataTable* DataTable;

public:
	UElementSubsystem();

	static FElementRowHandle GetElementHandle(const UWorld* InWorld, const FName& ElementName);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ApplyElement(AActor* InTarget, AActor* InInstigator, const FElementRowHandle& InElementRow);

	UO_WSElementReactor* GetReactor(const FElementRowHandle& InLeft, const FElementRowHandle& InRight) const;
	UO_WSElementReactor* GetReactor(TSubclassOf<UO_WSElementReactor> InReactorType) const;
};
