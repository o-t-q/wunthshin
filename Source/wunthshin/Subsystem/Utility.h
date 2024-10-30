#pragma once

struct FItemSubsystemUtility
{
	template <typename TableT, typename MetadatT>
	static void UpdateTable
	(
		UDataTable* InDataTable, 
		TMap<FName, MetadatT*>& OutMetadataMap
	)
	{
		check(InDataTable);

		TArray<TableT*> Rows;
		InDataTable->GetAllRows<TableT>(TEXT(""), Rows);

		for (const TableT* Row : Rows)
		{
			OutMetadataMap.Add(Row->ItemName);
			OutMetadataMap[Row->ItemName] = NewObject<MetadatT>();
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
