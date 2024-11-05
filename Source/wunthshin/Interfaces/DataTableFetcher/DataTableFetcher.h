// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataTableFetcher.generated.h"

struct FWeaponTableRow;
struct FItemTableRow;
struct FCharacterTableRow;

DECLARE_LOG_CATEGORY_CLASS(LogTableFetcher, Log, All);

// This class does not need to be modified.
UINTERFACE()
class UDataTableFetcher : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WUNTHSHIN_API IDataTableFetcher
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void UpdateDataTable(const FName& InRowName, const bool bForce = false) 
	{
		if (!DataTableRowHandle.IsNull() && !bForce) 
		{
			return;
		}

		const UObject* ObjectCast = Cast<UObject>(this);

		if (!ObjectCast)
		{
			check(false);
			return;
		}

		const UWorld* World = ObjectCast->GetWorld();
		ensureAlwaysMsgf(World, TEXT("Invalid World!"));
		DataTableRowHandle = GetRowHandle(ObjectCast, InRowName);
	}

	// 데이터 테이블 row 타입에 따라 핸들을 가져온 후 ApplyAsset을 호출
	void FetchAsset(const FName& InRowName)
	{
		if (DataTableRowHandle.IsNull()) 
		{
			UpdateDataTable(InRowName);
		}

		ApplyAsset(DataTableRowHandle.GetRow<FTableRowBase>(""));
	}

	// 상속 객체에 해당하는 서브 시스템 getter
	virtual UClass* GetSubsystemType() const = 0;
#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR 
	virtual UClass* GetEditorSubsystemType() const = 0;
#endif

	USubsystem* GetSubsystem() const;
	
	// 상속된 클래스가 주로 사용할 테이블 row 타입
	virtual UScriptStruct* GetTableType() const = 0;

	// 에셋의 데이터 테이블 핸들 getter
	FORCEINLINE FDataTableRowHandle GetDataTableHandle() const
	{
		ensureAlwaysMsgf(!DataTableRowHandle.IsNull(), TEXT("Data table might not be fetched before"));
		return DataTableRowHandle;
	}

protected:
	// 조회한 데이터 테이블의 데이터를 상속받은 클래스에서 사용
	virtual void ApplyAsset(const FTableRowBase* InRowPointer) = 0;
	
private:
	FDataTableRowHandle GetRowHandle(const UObject* InThisPointer, const FName& InRowName) const;

	// 에셋 이름과 타입을 기준으로 불러온 데이터 테이블 row의 핸들
	FDataTableRowHandle DataTableRowHandle;

	
};
