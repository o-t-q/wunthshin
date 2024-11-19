// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectSubsystem.h"
#include "wunthshin/Data/Effects/O_WSBaseEffect.h"

DEFINE_LOG_CATEGORY(LogEffectSubsystem);

UEffectSubsystem::UEffectSubsystem() : DataTable(nullptr) {}

void UEffectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_EffectTable.DT_EffectTable'")));
	check(DataTable);
	
	DataTableMapping.Emplace(FEffectTableRow::StaticStruct(), DataTable);

	// effector를 매번 사용할 때마다 instantiate하지 않고 한번 생성해서
	// 불러와 사용하도록, 서브시스템에서 미리 생성
	TArray<FEffectTableRow*> Rows;
	DataTable->GetAllRows<FEffectTableRow>(TEXT(""), Rows);

	// 전수 탐색 후 effector 생성
	for (const FEffectTableRow* Row : Rows)
	{
		UE_LOG(LogEffectSubsystem, Log, TEXT("%s: Effector added"), *Row->ItemEffect->GetName());
		PreinstantiatedEffectors.Add(Row->ItemEffect);
		PreinstantiatedEffectors[Row->ItemEffect] = NewObject<UO_WSBaseEffect>(this, Row->ItemEffect);
	}
}

UO_WSBaseEffect* UEffectSubsystem::GetEffector(const FEffectRowHandle& InRowHandle) const
{
	return GetEffector(GetRowValue<FEffectTableRow>(InRowHandle.Handle.RowName)->ItemEffect);
}

UO_WSBaseEffect* UEffectSubsystem::GetEffector(TSubclassOf<UO_WSBaseEffect> InEffectorType) const
{
	const bool bExist = PreinstantiatedEffectors.Contains(InEffectorType);
	// 알려지지 않은 reactor를 요청한 경우
	ensure(bExist);

	if (bExist) 
	{
		return PreinstantiatedEffectors[InEffectorType];
	}

	return nullptr;
}
