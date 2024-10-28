// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableFetcher.h"
#include "Editor.h"

FDataTableRowHandle IDataTableFetcher::GetRowHandleFromGameInstance(
	const UObject* InThisPointer, const FName& InRowName, const UWorld* World
) const
{
	const IDataTableQuery* Subsystem    = nullptr;
	const UGameInstance* GameInstance = World->GetGameInstance();

	if (!GameInstance)
	{
		UE_LOG(LogTableFetcher, Warning, TEXT("GameInstance is invalid"));
		return {};
	}

	const UScriptStruct* TableType = GetTableType();
		
	if (TableType == FItemTableRow::StaticStruct())
	{
		Subsystem = GameInstance->GetSubsystem<UItemSubsystem>();
	}
	else if (TableType == FWeaponTableRow::StaticStruct())
	{
		Subsystem = GameInstance->GetSubsystem<UWeaponSubsystem>();
	}
	else if (TableType == FCharacterTableRow::StaticStruct())
	{
		Subsystem = GameInstance->GetSubsystem<UCharacterSubsystem>();
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("Unknown table type"));
	}

	if (!Subsystem)
	{
		UE_LOG(LogTableFetcher, Warning, TEXT("Unable to get a subsystem"));
		return {};
	}

	UE_LOG(LogTableFetcher, Log, TEXT("Fetching asset of %s from %s"), *InThisPointer->GetName(), *InRowName.ToString())
	return Subsystem->GetRowHandle(TableType, InRowName);
}

// Add default functionality here for any IWSDataTableFetcher functions that are not pure virtual.
FDataTableRowHandle IDataTableFetcher::GetRowHandleFromEditorSubsystem(
	const UObject* InThisPointer, const FName& InRowName, const UWorld* World
) const
{
	const IDataTableQuery* Subsystem = nullptr;
	const UScriptStruct* TableType = GetTableType();
		
	if (TableType == FItemTableRow::StaticStruct())
	{
		Subsystem = GEditor->GetEditorSubsystem<UItemEditorSubsystem>();
	}
	else if (TableType == FWeaponTableRow::StaticStruct())
	{
		Subsystem = GEditor->GetEditorSubsystem<UWeaponEditorSubsystem>();
	}
	else if (TableType == FCharacterTableRow::StaticStruct())
	{
		Subsystem = GEditor->GetEditorSubsystem<UCharacterEditorSubsystem>();
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("Unknown table type"));
	}

	if (!Subsystem)
	{
		UE_LOG(LogTableFetcher, Warning, TEXT("Unable to get a subsystem"));
		return {};
	}

	UE_LOG(LogTableFetcher, Log, TEXT("Fetching asset of %s from %s"), *InThisPointer->GetName(), *InRowName.ToString())
	return Subsystem->GetRowHandle(TableType, InRowName);
}
