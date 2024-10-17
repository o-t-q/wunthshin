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
	
public:
	// 데이터 테이블의 타입, 다른 데이터 테이블을 쓸 경우 해당 타입을 재정의.
	using TRowTableType = FItemTableRow;
	static const FName CollisionComponentName;
	
	// Sets default values for this actor's properties
	AA_WSItem(const FObjectInitializer& ObjectInitializer);

	// 
	virtual void OnConstruction(const FTransform& Transform) override;

	// 충돌체 동적 생성 함수
	UFUNCTION(BlueprintCallable)
	void InitializeCollisionComponent(TSubclassOf<UShapeComponent> InClass);

	// 아이템 데이터 세팅
	virtual void ApplyAsset(const FDataTableRowHandle& InRowHandle) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
