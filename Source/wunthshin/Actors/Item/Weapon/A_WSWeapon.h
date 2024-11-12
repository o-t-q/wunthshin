// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wunthshin/Actors/Item/A_WSItem.h"
#include "wunthshin/Data/Elements/ElementRowHandle/ElementRowHandle.h"
#include "A_WSWeapon.generated.h"

struct FWeaponTableRow;
class UAnimMontage;
class UC_WSWeapon;

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API AA_WSWeapon : public AA_WSItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta=(AllowPrivateAccess = "true"))
	UC_WSWeapon* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element", meta=(AllowPrivateAccess = "true"))
	FElementRowHandle WeaponElement;
	
public:	
	// 자식 클래스가 Weapon Component를 override할 수 있도록 ObjectInitializer 생성자로
	AA_WSWeapon(const FObjectInitializer& ObjectInitializer);
	
	virtual UScriptStruct* GetTableType() const override;

	virtual UClass* GetSubsystemType() const override;
#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR 
	virtual UClass* GetEditorSubsystemType() const override;
#endif
	
	virtual void ApplyAsset(const FTableRowBase* InRowPointer) override;
	const FElementRowHandle& GetElement() const { return WeaponElement; }

	// 공격 모션 몽타주 getter
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<UAnimMontage*> GetAttackMontages() const { return AttackMontages; }
	
	
};
