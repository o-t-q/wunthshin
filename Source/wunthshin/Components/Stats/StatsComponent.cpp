#include "StatsComponent.h"
#include "Engine/DataTable.h"

#include "GameFramework/PawnMovementComponent.h"

#include "Logging/LogMacros.h"

#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"

DEFINE_LOG_CATEGORY(LogStatsComponent);

UStatsComponent::UStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // 스태미나 회복 및 감소 속도 초기화
    StaminaRecoveryRate = 5.0f; // 초당 회복량
    StaminaDepletionRate = 10.0f; // 빠르게 달릴 때 감소량
}

void UStatsComponent::BeginPlay()
{
    Super::BeginPlay(); // 부모 클래스의 BeginPlay 호출
}

void UStatsComponent::DecreaseHP(const float InValue)
{
    if (InValue < 0)
    {
        // 입력은 양수로 (오버/언더플로우 체크를 위해)
        ensureAlways(false);
        IncreaseHP(FMath::Abs(InValue));
        return;
    }

    if (FMath::Abs(std::numeric_limits<float>::min() + CurrentStats.HP) < InValue)
    {
        UE_LOG(LogStatsComponent, Warning, TEXT("%s: Underflow! assuming HP as 0"), *GetOwner()->GetName());
        CurrentStats.HP = 0;
        return;
    }

    CurrentStats.HP = FMath::Clamp(CurrentStats.HP - InValue, 0, CurrentStats.MaxHP);
}

void UStatsComponent::IncreaseHP(const float InValue)
{
    if (InValue < 0)
    {
        // 입력은 양수로 (오버/언더플로우 체크를 위해)
        ensureAlways(false);
        DecreaseHP(FMath::Abs(InValue));
        return;
    }

    if (std::numeric_limits<float>::max() - CurrentStats.HP < InValue)
    {
        UE_LOG(LogStatsComponent, Warning, TEXT("%s: Overflow! assuming HP as MaxHP"), *GetOwner()->GetName());
        CurrentStats.HP = CurrentStats.MaxHP;
        return;
    }

    CurrentStats.Stamina = FMath::Clamp(CurrentStats.HP + InValue, 0, CurrentStats.MaxHP);
}

void UStatsComponent::DecreaseStamina(const float InValue)
{
    if (InValue < 0)
    {
        // 입력은 양수로 (오버/언더플로우 체크를 위해)
        ensureAlways(false);
        IncreaseHP(FMath::Abs(InValue));
        return;
    }

    if (FMath::Abs(std::numeric_limits<float>::min() + CurrentStats.Stamina) < InValue)
    {
        UE_LOG(LogStatsComponent, Warning, TEXT("%s: Underflow! assuming Stamina as 0"), *GetOwner()->GetName());
        CurrentStats.Stamina = 0;
        return;
    }

    CurrentStats.Stamina = FMath::Clamp(CurrentStats.Stamina - InValue, 0, CurrentStats.Stamina);
}

void UStatsComponent::IncreaseStamina(const float InValue)
{
    if (InValue < 0)
    {
        // 입력은 양수로 (오버/언더플로우 체크를 위해)
        ensureAlways(false);
        DecreaseHP(FMath::Abs(InValue));
        return;
    }

    if (std::numeric_limits<float>::max() - CurrentStats.Stamina < InValue)
    {
        UE_LOG(LogStatsComponent, Warning, TEXT("%s: Overflow! assuming Stamina as 100"), *GetOwner()->GetName());
        CurrentStats.Stamina = 100.f;
        return;
    }

    CurrentStats.HP = FMath::Clamp(CurrentStats.Stamina + InValue, 0, 100.f);
}

void UStatsComponent::InitializeStats(const FCharacterStats& InInitialStats)
{
    CurrentStats = InInitialStats;
    CurrentStats.HP = CurrentStats.MaxHP;
    MovementStats = CurrentStats.Movement;
}

void UStatsComponent::UpdateStamina(float DeltaTime, bool bIsFastRunning)
{
    if (bIsFastRunning)
    {
        CurrentStats.Stamina -= StaminaDepletionRate * DeltaTime;
        CurrentStats.Stamina = FMath::Max(CurrentStats.Stamina, 0.0f); // 스태미나가 0 이하로 내려가지 않도록

        // 스태미나가 0이 되면 UnFastRun 호출
        if (CurrentStats.Stamina <= 0.0f)
        {
            CurrentStats.Stamina = 0.0f; // 스태미나를 0으로 클램프
            if (ICommonPawn* Pawn = Cast<ICommonPawn>(GetOwner()))
            {
                Pawn->HandleStaminaDepleted(); // 공개 메서드 호출
            }
        }
    }
    else
    {

        float check = StaminaRecoveryRate * DeltaTime;
        CurrentStats.Stamina += StaminaRecoveryRate * DeltaTime;
        CurrentStats.Stamina = FMath::Min(CurrentStats.Stamina, 100.0f); // 최대 스태미나로 제한
    }
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 여기서 스태미나 업데이트 로직을 추가
    if (const ICommonPawn* Pawn = Cast<ICommonPawn>(GetOwner()))
    {
        UpdateStamina(DeltaTime, Pawn->IsFastRunning());
    }
}
