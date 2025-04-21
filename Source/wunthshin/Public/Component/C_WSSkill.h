// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Components/ActorComponent.h"
#include "Data/Skill/SkillRowHandle.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	bool bActive;

	FEnhancedInputActionEventBinding* SkillKeyBinding;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_CastSkill();

	UFUNCTION()
	void CastSkill();

	// 초기 입력 컴포넌트 설정 함수
	// BeginPlay보다 OnPossessedPawnChanged이 먼저 발생함에 따라
	// 시작 캐릭터의 입력 컴포넌트 설정이 누락됨
	void InitializeInputComponent();

	// 캐릭터 변경에 따른 스킬 바인딩 Delegate 추가/삭제
	UFUNCTION()
	void ReconfigureInputComponent(APawn* OldPawn, APawn* NewPawn);

public:
	void SetCharacterSkill(const FSkillRowHandle& InCharacterSkill) { CharacterSkill = InCharacterSkill; }

	void SetSkillActive(const bool InValue) { bActive = InValue; }
	bool GetSkillActive() const { return bActive; }
};