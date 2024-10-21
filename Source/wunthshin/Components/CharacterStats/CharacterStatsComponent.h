#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Logging/LogMacros.h"
#include "CharacterStatsComponent.generated.h"

// 캐릭터의 스탯을 정의하는 구조체
USTRUCT(BlueprintType)
struct FCharacterStats : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
    float MaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
    float Stamina;

    // 추가적인 스탯을 여기서 정의할 수 있습니다.
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WUNTHSHIN_API UCharacterStatsComponent : public UActorComponent
{
    GENERATED_BODY()

private:
    // 스태미나 회복 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
    float StaminaRecoveryRate;

    // 스태미나 감소 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
    float StaminaDepletionRate;

public:
    // 생성자
    UCharacterStatsComponent();

protected:
    virtual void BeginPlay() override;

public:
    // 캐릭터의 스탯 데이터 테이블
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    UDataTable* CharacterStatsTable;

    // 캐릭터 스탯
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    FCharacterStats CurrentStats;

    // 스탯 초기화 함수
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void InitializeStats();

    // 스태미나 업데이트 함수
    void UpdateStamina(float DeltaTime, bool bIsFastRunning);

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
