#pragma once

class USG_WSItemMetadata;

struct FItemSubsystemUtility
{
	template <typename TableT, typename MetadataT>
	static void UpdateTable(UDataTable* InDataTable, TMap<FName, MetadataT*>& OutMetadataMap)
	{
		check(InDataTable);

		TArray<TableT*> Rows;
		InDataTable->GetAllRows<TableT>(TEXT(""), Rows);

		for (const TableT* Row : Rows)
		{
			OutMetadataMap.Add(Row->ItemName);
			OutMetadataMap[Row->ItemName] = NewObject<MetadataT>();
			OutMetadataMap[Row->ItemName]->AssetName = Row->ItemName;
			OutMetadataMap[Row->ItemName]->ItemType = Row->ItemType;
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

	template <typename SubsystemGameInstanceT, typename SubsystemEditorT, typename MetadataT>
	static MetadataT* GetMetadata(const UWorld* InWorld, const FName& InAssetName)
	{
#ifdef WITH_EDITOR
		if (GIsEditor)
		{
			return GEditor->GetEditorSubsystem<SubsystemEditorT>()->GetMetadata(InAssetName);
		}
#endif
		if (!InWorld->IsEditorWorld())
		{
			return InWorld->GetGameInstance()->GetSubsystem<SubsystemGameInstanceT>()->GetMetadata(InAssetName);
		}

		// World가 존재하지 않음
		check(false);
		return nullptr;
	}
};
