#include "CharacterStatsComponent.h"
#include "Engine/DataTable.h"
#include "Logging/LogMacros.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "wunthshin/Actors/AA_WSCharacter.h" 


UCharacterStatsComponent::UCharacterStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    // 데이터 테이블 초기화
    CharacterStatsTable = nullptr;

    // 데이터 테이블을 에디터에서 설정할 수 있도록 초기화
    static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("DataTable'/Game/DataTable/DT_CharacterStatsTable.DT_CharacterStatsTable'"));
    if (DataTableAsset.Succeeded())
    {
        CharacterStatsTable = DataTableAsset.Object;
        UE_LOG(LogTemp, Warning, TEXT("DataTable loaded successfully: %s"), *CharacterStatsTable->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load DataTable!"));
    }

    // 스태미나 회복 및 감소 속도 초기화
    StaminaRecoveryRate = 5.0f; // 초당 회복량
    StaminaDepletionRate = 10.0f; // 빠르게 달릴 때 감소량
}

void UCharacterStatsComponent::BeginPlay()
{
    Super::BeginPlay(); // 부모 클래스의 BeginPlay 호출

    InitializeStats(); // 스탯 초기화
}

void UCharacterStatsComponent::InitializeStats()
{
    if (CharacterStatsTable)
    {
        // "Player" 항목의 스탯을 찾는다
        FCharacterStats* Stats = CharacterStatsTable->FindRow<FCharacterStats>(TEXT("Player"), TEXT("Stats"));

        if (Stats)
        {
            CurrentStats.MaxHP = Stats->MaxHP;
            CurrentStats.Stamina = Stats->Stamina;
            UE_LOG(LogTemp, Warning, TEXT("MaxHP: %f, Stamina: %f"), CurrentStats.MaxHP, CurrentStats.Stamina);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Row not found in CharacterStatsTable!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CharacterStatsTable is null!"));
    }
}

void UCharacterStatsComponent::UpdateStamina(float DeltaTime, bool bIsFastRunning)
{
    if (bIsFastRunning)
    {
        CurrentStats.Stamina -= StaminaDepletionRate * DeltaTime;
        CurrentStats.Stamina = FMath::Max(CurrentStats.Stamina, 0.0f); // 스태미나가 0 이하로 내려가지 않도록

        // 스태미나가 0이 되면 UnFastRun 호출
        if (CurrentStats.Stamina <= 0.0f)
        {
            CurrentStats.Stamina = 0.0f; // 스태미나를 0으로 클램프
            AA_WSCharacter* Character = Cast<AA_WSCharacter>(GetOwner());
            if (Character)
            {
                Character->HandleStaminaDepleted(); // 공개 메서드 호출
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

void UCharacterStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 여기서 스태미나 업데이트 로직을 추가
    AA_WSCharacter* Character = Cast<AA_WSCharacter>(GetOwner());
    if (Character)
    {
        if (FMath::Abs(Character->GetMovementComponent()->Velocity.Dot(FVector::ForwardVector + FVector::RightVector)) > 0.f)
        {
            UpdateStamina(DeltaTime, Character->IsFastRunning());
        }
    }
}
