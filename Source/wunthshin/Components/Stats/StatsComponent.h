#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Logging/LogMacros.h"
#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"
#include "StatsComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStatsComponent, Log, All);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WUNTHSHIN_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()
	
	// 스태미나 회복 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float StaminaRecoveryRate;

	// 스태미나 감소 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float StaminaDepletionRate;

public:
	// 생성자
	UStatsComponent();

protected:
	// 캐릭터 스탯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	FCharacterStats CurrentStats;
	
	virtual void BeginPlay() override;

public:
	// 체력감소, 입력 값은 양수로
	UFUNCTION(BlueprintCallable)
	void DecreaseHP(const float InValue);

	// 체력증가, 입력 값은 양수로
	UFUNCTION(BlueprintCallable)
	void IncreaseHP(const float InValue);

	// 전체 체력과 현재 체력 비율
	UFUNCTION(BlueprintCallable)
	float GetHPRatio() const
	{
		if (CurrentStats.HP == 0 || CurrentStats.MaxHP == 0)
		{
			return 0.f;
		}
        
		return CurrentStats.HP / CurrentStats.MaxHP;
	}

	// 스탯 초기화 함수
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void InitializeStats(const FCharacterStats& InInitialStats);

	// 스태미나 업데이트 함수
	void UpdateStamina(float DeltaTime, bool bIsFastRunning);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	float GetHP() const { return CurrentStats.HP; }
};
