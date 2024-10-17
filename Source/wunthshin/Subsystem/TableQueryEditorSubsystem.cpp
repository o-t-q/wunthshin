// Fill out your copyright notice in the Description page of Project Settings.


#include "TableQueryEditorSubsystem.h"
void UTableQueryEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// 자식 클래스에서 Object를 안넣었거나, 찾는 테이블이 없는 경우
	ensure(DataTable);
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
