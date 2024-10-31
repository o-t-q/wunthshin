// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableFetcher.h"
#include "Editor.h"

#include "wunthshin/Subsystem/Utility.h"

USubsystem* IDataTableFetcher::GetSubsystem() const
{
	const UWorld* World = Cast<UObject>(this)->GetWorld();
	USubsystem* Subsystem = nullptr;

#ifdef WITH_EDITOR
	Subsystem = SUBSYSTEM_EDITOR_BRANCHING_REFLECT(World, GetEditorSubsystemType(), GetSubsystemType());
#else
	Subsystem = GetWorld()->GetGameInstance()->GetSubsystemBase(GetSubsystemType());
#endif
	
	check(Subsystem);

	return Subsystem;
}

FDataTableRowHandle IDataTableFetcher::GetRowHandle(
	const UObject* InThisPointer, const FName& InRowName
) const
{
	USubsystem* Subsystem = GetSubsystem();
	check(Subsystem);
	UE_LOG(LogDataTable, Log, TEXT("Accessing the Subsystem: %s"), *Subsystem->GetName());
	
	const IDataTableQuery* TableQuery = Cast<IDataTableQuery>(Subsystem);
	check(TableQuery);

	UE_LOG(LogDataTable, Log, TEXT("Access data table from %s: %s"), *InThisPointer->GetName(), *InRowName.ToString());
	return TableQuery->GetRowHandle(GetTableType(), InRowName);
}
