// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableQuery.h"


// Add default functionality here for any IDataTableQuery functions that are not pure virtual.
FDataTableRowHandle IDataTableQuery::GetRowHandle(const UScriptStruct* InRequestType, const FName& InRowName) const
{
	// 데이터 테이블 인터페이스를 상속받는 클래스가 매핑을 해둬야 함
	ensure(!DataTableMapping.IsEmpty());
	
	for (decltype(DataTableMapping)::TRangedForConstIterator It = DataTableMapping.begin(); It != DataTableMapping.end(); ++It)
	{
		if (It->Key == InRequestType)
		{
			FDataTableRowHandle Handle;
			ensureAlways(DataTableMapping.Contains(InRequestType));
			Handle.DataTable = *DataTableMapping.Find(InRequestType);
			Handle.RowName = InRowName;
			return Handle;
		}
	}

	return {};
}
