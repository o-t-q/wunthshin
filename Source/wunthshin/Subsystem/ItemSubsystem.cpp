// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSubsystem.h"

UItemSubsystem::UItemSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemTable.DT_ItemTable'"));
	check(Table.Object);
	ItemDataTable = Table.Object;
}

FDataTableRowHandle UItemSubsystem::FindItem(const FName& InKey)
{
	FDataTableRowHandle RowHandle;
#if WITH_EDITOR
	bool bFind = false;
	if (ItemDataTable->GetRowMap().Find(InKey))
	{
		check(!bFind);
		bFind = true;
		RowHandle.DataTable = ItemDataTable;
		RowHandle.RowName = InKey;
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
