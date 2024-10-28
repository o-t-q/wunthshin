// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataTableQuery.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDataTableQuery : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WUNTHSHIN_API IDataTableQuery
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	template <typename TableT> requires (std::is_base_of_v<FTableRowBase, TableT>)
	TableT* GetRowValue(const FName& InRowName) const
	{
		const FDataTableRowHandle Handle = GetRowHandle(TableT::StaticStruct(), InRowName);
		// 핸들 조회 실패
		ensureAlways(!Handle.IsNull());
		return Handle.GetRow<TableT>(TEXT(""));
	}
	
	FDataTableRowHandle GetRowHandle(const UScriptStruct* InRequestType, const FName& InRowName) const;

	
protected:
	// 구조체 타입맵
	TMap<const UScriptStruct*, UDataTable*> DataTableMapping;
	
};
