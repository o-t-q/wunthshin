#pragma once

#include "Enums.h"
#include "Data/Effect/EffectParameter.h"
#include "Data/Effect/EffectTableRow.h"

#include "ItemTableRow.generated.h"

USTRUCT()
struct WUNTHSHIN_API FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// 등급
	UPROPERTY(EditAnywhere)
	ERarity ItemRarity;
	
	// ItemType(소모, 장비, 기타 등)
	UPROPERTY(EditAnywhere)
	EItemType ItemType;

	// ItemName을 Key값으로 사용
	UPROPERTY(EditAnywhere)
	FName ItemName;

	UPROPERTY(EditAnywhere)
	FName ItemDescription;

	UPROPERTY(EditAnywhere)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;
	
	UPROPERTY(EditAnywhere)
	bool bCollision;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCollision", EditConditionHides))
	TSubclassOf<UShapeComponent> CollisionShape;

	UPROPERTY(EditAnywhere)
	FTransform MeshOffset;

	UPROPERTY()
	bool bBox;

	UPROPERTY()
	bool bSphere;

	UPROPERTY()
	bool bCapsule;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bBox && bCollision", EditConditionHides))
	FVector BoxExtents;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bSphere && bCollision", EditConditionHides))
	float Radius;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCapsule && bCollision", EditConditionHides))
	float CapsuleRadius;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCapsule", EditConditionHides))
	float CapsuleHeight;

	UPROPERTY(EditAnywhere, meta=(RowType="/Script/wunthshin.EffectTableRow"))
	FDataTableRowHandle ItemEffect;

	// 아이템 효과에 매개변수로 주어질 값
	UPROPERTY(EditAnywhere)
	FEffectParameter ItemParameter;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};

USTRUCT()
struct WUNTHSHIN_API FWeaponTableRow : public FItemTableRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
	UPROPERTY(EditAnywhere)
	int32 Damage;

	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/wunthshin.ElementTableRow"))
	FDataTableRowHandle Element;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontages;
	
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};