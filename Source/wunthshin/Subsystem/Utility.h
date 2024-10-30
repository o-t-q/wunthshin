#pragma once
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"

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
		if (InWorld->IsEditorWorld())
		{
			Subsystem = GEditor->GetEditorSubsystemBase(InDataTableFetcher->GetEditorSubsystemType());
		}
#endif
		if (InWorld->IsGameWorld())
		{
			Subsystem = InWorld->GetGameInstance()->GetSubsystemBase(InDataTableFetcher->GetSubsystemType());
		}

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
