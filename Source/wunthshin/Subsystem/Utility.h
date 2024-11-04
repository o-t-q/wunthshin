#pragma once
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"

#ifdef WITH_EDITOR
struct FEditorSubsystemBranching
{
	template <typename EditorWorldType, typename GameWorldType>
	static USubsystem* GetTemplate(const UWorld* InWorld)
	{
		// Inactive는 에디터에서 지금 수정하지 않는 상태에 들어간 경우에 발생함
		if (InWorld->IsEditorWorld() || InWorld->WorldType == EWorldType::Inactive)
		{
			return GEditor->GetEditorSubsystem<EditorWorldType>();
		}
		if (InWorld->IsGameWorld())
		{
			return InWorld->GetGameInstance()->GetSubsystem<GameWorldType>();
		}

		return nullptr;
	}

	static USubsystem* GetReflection(const UWorld* InWorld, UClass* InEditorType, UClass* InGameWorldType)
	{
		// Inactive는 에디터에서 지금 수정하지 않는 상태에 들어간 경우에 발생함
		if (InWorld->IsEditorWorld() || InWorld->WorldType == EWorldType::Inactive)
		{
			return GEditor->GetEditorSubsystemBase(InEditorType);
		}
		if (InWorld->IsGameWorld())
		{
			return InWorld->GetGameInstance()->GetSubsystemBase(InGameWorldType);
		}

		return nullptr;
	}
};

#define SUBSYSTEM_EDITOR_BRANCHING_TYPE(World, SubsystemName) \
	FEditorSubsystemBranching::GetTemplate<U##SubsystemNameEditorSubsystem, U##SubsystemNameSubsystem>(World);
#define SUBSYSTEM_EDITOR_BRANCHING_REFLECT(World, EditorWorldType, GameWorldType) \
	FEditorSubsystemBranching::GetReflection(World, EditorWorldType, GameWorldType);
#endif

class IDataTableFetcher;

struct FItemSubsystemUtility
{
	template <typename TableT, typename MetadataT>
	static void UpdateTable
	(
		UDataTable* InDataTable, 
		TMap<FName, MetadataT*>& OutMetadataMap
	)
	{
		check(InDataTable);
		OutMetadataMap.Empty();
		
		TArray<TableT*> Rows;
		InDataTable->GetAllRows<TableT>(TEXT(""), Rows);

		for (const TableT* Row : Rows)
		{
			OutMetadataMap.Add(Row->ItemName);
			OutMetadataMap[Row->ItemName] = NewObject<MetadataT>();
			OutMetadataMap[Row->ItemName]->AssetName = Row->ItemName;
			OutMetadataMap[Row->ItemName]->ItemType = Row->ItemType;
			OutMetadataMap[Row->ItemName]->ItemIcon = Row->ItemIcon;
			OutMetadataMap[Row->ItemName]->ItemDescription = Row->ItemDescription;
			OutMetadataMap[Row->ItemName]->ItemEffect = FEffectRowHandle(Row->ItemEffect);
			OutMetadataMap[Row->ItemName]->ItemParameter = Row->ItemParameter;
		}
	}

	template <typename MetadataT>
	static MetadataT* GetMetadataTemplate(const TMap<FName, MetadataT*>& InMetadataMap, const FName& InAssetName)
	{
		if (InMetadataMap.Contains(InAssetName))
		{
			return InMetadataMap[InAssetName];
		}

		// 메타데이터가 생성되지 않았을 경우
		check(false);
		return nullptr;
	}

	template <typename MetadataT>
	static MetadataT* GetMetadata(const UWorld* InWorld, const IDataTableFetcher* InDataTableFetcher, const FName& InAssetName)
	{
		USubsystem* Subsystem = nullptr;
		MetadataT* OutValue = nullptr;
		
#ifdef WITH_EDITOR
		Subsystem = SUBSYSTEM_EDITOR_BRANCHING_REFLECT(InWorld, InDataTableFetcher->GetEditorSubsystemType(), InDataTableFetcher->GetSubsystemType());
#else
		Subsystem = InWorld->GetGameInstance()->GetSubsystemBase(InDataTableFetcher->GetSubsystemType());
#endif

		check(Subsystem);
		IItemMetadataGetter* MetadataGetter = Cast<IItemMetadataGetter>(Subsystem);
		check(MetadataGetter);

		if (!MetadataGetter || !Subsystem)
		{
			return nullptr;
		}
		
		OutValue = MetadataGetter->GetMetadata(InAssetName);
		return OutValue;
	}
};
