// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableFetcher.h"
#include "Editor.h"

FDataTableRowHandle IDataTableFetcher::GetRowHandle(
	const UWorld* InWorld, const UObject* InThisPointer, const FName& InRowName
) const
{
	USubsystem* Subsystem = nullptr;
	
	if (InWorld->IsGameWorld())
	{
		UE_LOG(LogDataTable, Log, TEXT("Accessing the GameInstance Subsystem: %s"), *GetSubsystemType()->GetName());
		Subsystem = InWorld->GetGameInstance()->GetSubsystemBase(GetSubsystemType());
	}
	
#ifdef WITH_EDITOR
	if (!Subsystem && InWorld->IsEditorWorld())
	{
		UE_LOG(LogDataTable, Log, TEXT("Accessing the Editor Subsystem: %s"), *GetSubsystemType()->GetName());
		Subsystem = GEditor->GetEditorSubsystemBase(GetEditorSubsystemType()); 
	}
#endif
	
	check(Subsystem);
	const IDataTableQuery* TableQuery = Cast<IDataTableQuery>(Subsystem);
	check(TableQuery);

	UE_LOG(LogDataTable, Log, TEXT("Access data table from %s: %s"), *InThisPointer->GetName(), *InRowName.ToString());
	return TableQuery->GetRowHandle(GetTableType(), InRowName);
}
