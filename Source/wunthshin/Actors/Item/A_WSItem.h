// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"

#include "A_WSItem.generated.h"

class UProjectileMovementComponent;
struct FItemTableRow;
class UC_WSPickUp;
class USG_WSItemMetadata;
class UWidgetComponent;

UCLASS()
class WUNTHSHIN_API AA_WSItem : public AActor, public IDataTableFetcher
{
	GENERATED_BODY()

	// 매시 충돌체를 대체하는 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision", meta=(AllowPrivateAccess = "true"))
	UShapeComponent* CollisionComponent;

	// 줍기/버리기 상호작용을 위한 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PickUp", meta=(AllowPrivateAccess = "true"))
	UC_WSPickUp* PickUpComponent;

	// 아이템을 표시해주는 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ItemNotifyWidget;

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

	// 아이템을 주울 수 있다는 알림을 띄우는 위젯을 관리하는 컴포넌트
	FORCEINLINE UWidgetComponent* GetItemNotifyWidget() const { return ItemNotifyWidget; }

	void SetAssetName(const FName& InAssetName) { AssetName = InAssetName; }
	FName GetAssetName() const { return AssetName; }
	
	const USG_WSItemMetadata* GetItemMetadata() const;

protected:
	// 아이템의 메타데이터 (테이블에서 생성한 정적변수, Destroy 하면 안됨!)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metadata")
	const USG_WSItemMetadata* ItemMetadata;
	
	// 매시
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* MeshComponent;

	// 아이템 물리충돌
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	UProjectileMovementComponent* ItemPhysics;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 충돌체 업데이트 (상속 클래스가 다른 테이블을 사용하고 Item의 데이터 테이블에서 충돌 적용이 필요한 경우)
	void UpdateCollisionFromDataTable(const FItemTableRow* Data);

};
