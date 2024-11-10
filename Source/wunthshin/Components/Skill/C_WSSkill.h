// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Components/ActorComponent.h"
#include "wunthshin/Data/Skills/SkillRowHandle/SkillRowHandle.h"
#include "C_WSSkill.generated.h"

class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogSkillComponent, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WUNTHSHIN_API UC_WSSkill : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UC_WSSkill();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* SkillMappingContext;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess = "true"))
	UInputAction* SkillAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta=(AllowPrivateAccess=true))
	FSkillRowHandle CharacterSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	bool bActive;

	FEnhancedInputActionEventBinding* SkillKeyBinding;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
	UFUNCTION()
	void CastSkill();

	void SetupInputComponent();
	void RemoveInputComponent();

public:
	void SetCharacterSkill(const FSkillRowHandle& InCharacterSkill) { CharacterSkill = InCharacterSkill; }

	void SetSkillActive(const bool InValue) { bActive = InValue; }
	bool GetSkillActive() const { return bActive; }
};