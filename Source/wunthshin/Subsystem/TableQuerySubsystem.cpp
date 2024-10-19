// Fill out your copyright notice in the Description page of Project Settings.


#include "TableQuerySubsystem.h"

void UTableQueryGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

FDataTableRowHandle UTableQueryGameInstanceSubsystem::FindItem(const FName& InKey) const
{
	FDataTableRowHandle RowHandle;
#if WITH_EDITOR
	if (DataTable->GetRowMap().Find(InKey))
	{
		RowHandle.DataTable = DataTable;
		RowHandle.RowName = InKey;
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("Unable to find row from table"));
	}
#else
	if (ItemDataTable->GetRowMap().Find(InKey))
	{
		RowHandle.DataTable = ItemDataTable;
		RowHandle.RowName = InKey;
	}
#endif // WITH_EDITOR

	return RowHandle;
}

void UTableQueryEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

FDataTableRowHandle UTableQueryEditorSubsystem::FindItem(const FName& InKey) const
{
	FDataTableRowHandle RowHandle;
#if WITH_EDITOR
	if (DataTable->GetRowMap().Find(InKey))
	{
		RowHandle.DataTable = DataTable;
		RowHandle.RowName = InKey;
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("Unable to find row from table"));
	}
#else
	if (ItemDataTable->GetRowMap().Find(InKey))
	{
		RowHandle.DataTable = ItemDataTable;
		RowHandle.RowName = InKey;
	}
#endif // WITH_EDITOR

	return RowHandle;
}

