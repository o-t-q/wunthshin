// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"

#include "A_WSItem.generated.h"

struct FItemTableRow;
class UC_WSPickUp;

UCLASS()
class WUNTHSHIN_API AA_WSItem : public AActor, public IDataTableFetcher
{
	GENERATED_BODY()

	// 물체 오프셋 설정을 위한 더미 루트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pivot", meta=(AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComponent;

	// 매시
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	// 매시 충돌체를 대체하는 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision", meta=(AllowPrivateAccess = "true"))
	UShapeComponent* CollisionComponent;

	// 줍기/버리기 상호작용을 위한 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PickUp", meta=(AllowPrivateAccess = "true"))
	UC_WSPickUp* PickUpComponent;

	// 아이템 정보를 불러오기 위한 핸들
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	FName AssetName;
	
	// 충돌체 동적 생성 후 호출
	void InitializeCollisionLazy() const;

	// 매시의 Bound에 따라 업데이트
	void FitCollisionToMesh() const;
	
public:
	static const FName CollisionComponentName;
	
	// Sets default values for this actor's properties
	AA_WSItem(const FObjectInitializer& ObjectInitializer);

	// 
	virtual void OnConstruction(const FTransform& Transform) override;

	// 충돌체 동적 생성 함수
	UFUNCTION(BlueprintCallable)
	void InitializeCollisionComponent(TSubclassOf<UShapeComponent> InClass);

	virtual UScriptStruct* GetTableType() const override;

	// 아이템 데이터 세팅
	virtual void ApplyAsset(const FDataTableRowHandle& InRowHandle) override;

	FORCEINLINE UStaticMeshComponent* GetMesh() const { return MeshComponent; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual FName GetAssetName() const { return AssetName; }

	// 충돌체 업데이트 (상속 클래스가 다른 테이블을 사용하고 Item의 데이터 테이블에서 충돌 적용이 필요한 경우)
	void UpdateCollisionFromDataTable(const FItemTableRow* Data);

};
