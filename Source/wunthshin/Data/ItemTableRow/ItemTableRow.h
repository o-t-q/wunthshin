#pragma once

#include "wunthshin/Enums.h"

#include "ItemTableRow.generated.h"

class USG_WSItemMetadata;

USTRUCT()
struct WUNTHSHIN_API FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// ItemType(소모, 장비, 기타 등)
	UPROPERTY(EditAnywhere)
	EItemType ItemType;

	// ItemName을 Key값으로 사용
	UPROPERTY(EditAnywhere)
	FName ItemName;

	UPROPERTY(EditAnywhere)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UShapeComponent> CollisionShape;

	UPROPERTY(EditAnywhere)
	FTransform MeshOffset;

	UPROPERTY()
	bool bBox;

	UPROPERTY()
	bool bSphere;

	UPROPERTY()
	bool bCapsule;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bBox", EditConditionHides))
	FVector BoxExtents;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bSphere", EditConditionHides))
	float Radius;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCapsule", EditConditionHides))
	float CapsuleRadius;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCapsule", EditConditionHides))
	float CapsuleHeight;

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

	UPROPERTY(EditAnywhere)
	UAnimMontage* DefaultAttackMontage;
	
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};