#include "CharacterStatsComponent.h"
#include "Engine/DataTable.h"
#include "Logging/LogMacros.h"
#include "wunthshin/Actors/AA_WSCharacter.h" 


UCharacterStatsComponent::UCharacterStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 데이터 테이블을 에디터에서 설정할 수 있도록 초기화
    // /Script/Engine.DataTable'/Game/Characters/CharacterStatsTable/CharacterStatsTable.CharacterStatsTable'
    static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("DataTable'/Game/Characters/CharacterStatsTable/CharacterStatsTable.CharacterStatsTable'"));
    if (DataTableAsset.Succeeded())
    {
        CharacterStatsTable = DataTableAsset.Object;
        UE_LOG(LogTemp, Warning, TEXT("DataTable loaded successfully: %s"), *CharacterStatsTable->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load DataTable!"));
    }
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
