// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TableQuerySubsystem.generated.h"

/**
 * 데이터 테이블로부터 정보를 가져오는 게임 인스턴스 서브 시스템의 공통 부모 클래스
 */
UCLASS()
class WUNTHSHIN_API UTableQueryGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table")
	UDataTable* DataTable;

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	FDataTableRowHandle FindItem(const FName& InKey) const;
	
};

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UTableQueryEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table")
	UDataTable* DataTable;

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	FDataTableRowHandle FindItem(const FName& InKey) const;
};
