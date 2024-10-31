// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "wunthshin/Subsystem/CharacterSubsystem/CharacterSubsystem.h"
#include "wunthshin/Subsystem/ItemSubsystem/ItemSubsystem.h"
#include "wunthshin/Subsystem/WeaponSubsystem/WeaponSubsystem.h"

#include "Editor/UnrealEd/Public/Editor.h"

#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"

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
	// 데이터 테이블 row 타입에 따라 핸들을 가져온 후 ApplyAsset을 호출
	void FetchAsset(const UObject* InThisPointer, const FName& InRowName)
	{
		const UWorld* World = InThisPointer->GetWorld();
		ensureAlwaysMsgf(World, TEXT("Invalid World!"));
		DataTableRowHandle = GetRowHandle(World, InThisPointer, InRowName);
		ApplyAsset(DataTableRowHandle);
	}

	// 상속 객체에 해당하는 서브 시스템 getter
	virtual UClass* GetSubsystemType() const = 0;
#ifdef WITH_EDITOR
	virtual UClass* GetEditorSubsystemType() const = 0;
#endif
	
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
	virtual void ApplyAsset(const FDataTableRowHandle& InRowHandle) = 0;
	
private:
	FDataTableRowHandle GetRowHandle(const UWorld* InWorld, const UObject* InThisPointer, const FName& InRowName) const;

	// 에셋 이름과 타입을 기준으로 불러온 데이터 테이블 row의 핸들
	FDataTableRowHandle DataTableRowHandle;

	
};
