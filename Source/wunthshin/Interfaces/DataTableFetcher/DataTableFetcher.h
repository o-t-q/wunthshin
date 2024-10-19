// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "wunthshin/Subsystem/CharacterSubsystem/CharacterSubsystem.h"
#include "wunthshin/Subsystem/ItemSubsystem/ItemSubsystem.h"
#include "wunthshin/Subsystem/WeaponSubsystem/WeaponSubsystem.h"

#include "Editor/UnrealEd/Public/Editor.h"

#include "wunthshin/Data/CharacterTableRow.h"
#include "wunthshin/Data/ItemTableRow.h"

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
	// 변수값이 있는 공통 상속 클래스가 없기 때문에 ThisT template 유지
	template <typename ThisT> requires (std::is_base_of_v<UObject, ThisT>)
	void FetchAsset(ThisT* InThisPointer, const FName& InRowName)
	{
		const UWorld*                           World     = InThisPointer->GetWorld();
		ensureAlwaysMsgf(World, TEXT("Invalid World!"));

#ifdef WITH_EDITOR
		if (World->IsEditorWorld() || !World)
		{
			InThisPointer->DataTableRowHandle = GetRowHandleFromEditorSubsystem(InThisPointer, InRowName, World);
		}
#endif
		if (!World->IsEditorWorld())
		{
			InThisPointer->DataTableRowHandle = GetRowHandleFromGameInstance(InThisPointer, InRowName, World);
		}
		
		ApplyAsset(InThisPointer->DataTableRowHandle);
	}

	virtual UScriptStruct* GetTableType() const = 0;

protected:
	// 에셋의 데이터 테이블 핸들 getter
	FORCEINLINE FDataTableRowHandle GetDataTableHandle() const
	{
		ensureAlwaysMsgf(!DataTableRowHandle.IsNull(), TEXT("Data table might not be fetched before"));
		return DataTableRowHandle;
	}
	
	// 조회한 데이터 테이블의 데이터를 상속받은 클래스에서 사용
	virtual void ApplyAsset(const FDataTableRowHandle& InRowHandle) = 0;
	
private:
	FDataTableRowHandle GetRowHandleFromGameInstance(const UObject* InThisPointer, const FName& InRowName, const UWorld* World) const;
	FDataTableRowHandle GetRowHandleFromEditorSubsystem(const UObject* InThisPointer, const FName& InRowName, const UWorld* World) const;
	
	// 에셋 이름과 타입을 기준으로 불러온 데이터 테이블 row의 핸들
	FDataTableRowHandle DataTableRowHandle;

	
};
