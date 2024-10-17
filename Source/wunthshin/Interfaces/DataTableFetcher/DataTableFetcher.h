// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "wunthshin/Subsystem/CharacterSubsystem/CharacterSubsystem.h"
#include "wunthshin/Subsystem/ItemSubsystem/ItemSubsystem.h"
#include "wunthshin/Subsystem/WeaponSubsystem/WeaponSubsystem.h"

#include "Editor/UnrealEd/Public/Editor.h"

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
	template <typename TableType, typename ThisT> requires (std::is_base_of_v<FTableRowBase, TableType> && std::is_base_of_v<UObject, ThisT>)
	void FetchAsset(ThisT* InThisPointer, const FName& InRowName)
	{
		const UTableQueryGameInstanceSubsystem* Subsystem = nullptr;
		const UWorld*                           World     = InThisPointer->GetWorld();
#ifdef WITH_EDITOR
		// todo: PIE가 실행되기 전에 Game instance가 존재하지 않음
#endif
		ensureAlwaysMsgf(World, TEXT("Invalid World!"));
		const UGameInstance* GameInstance = World->GetGameInstance();

		if (!GameInstance)
		{
			UE_LOG(LogTableFetcher, Warning, TEXT("GameInstance is invalid"));
			return;
		}
		
		if constexpr (std::is_same_v<TableType, FItemTableRow>)
		{
			Subsystem = GameInstance->GetSubsystem<UItemSubsystem>();
		}
		else if constexpr (std::is_same_v<TableType, FWeaponTableRow>)
		{
			Subsystem = GameInstance->GetSubsystem<UWeaponSubsystem>();
		}
		else if constexpr (std::is_same_v<TableType, FCharacterTableRow>)
		{
			Subsystem = GameInstance->GetSubsystem<UCharacterSubsystem>();
		}
		else
		{
			static_assert("Unknown table row type given");
		}

		if (!Subsystem)
		{
			UE_LOG(LogTableFetcher, Warning, TEXT("Unable to get a subsystem"));
			return;
		}

		UE_LOG(LogTableFetcher, Log, TEXT("Fetching asset of %s from %s"), *InThisPointer->GetName(), *InRowName.ToString())
		InThisPointer->DataTableRowHandle = Subsystem->FindItem(InRowName);
		ApplyAsset(InThisPointer->DataTableRowHandle);
	}

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
	// 에셋 이름과 타입을 기준으로 불러온 데이터 테이블 row의 핸들
	FDataTableRowHandle DataTableRowHandle;

	
};
