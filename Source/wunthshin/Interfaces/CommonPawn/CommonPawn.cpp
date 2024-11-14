// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonPawn.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"
#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "wunthshin/wunthshinPlayerState.h"
#include "wunthshin/Actors/Item/Weapon/A_WSWeapon.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Components/Skill/C_WSSkill.h"
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"
#include "wunthshin/Interfaces/DataTableQuery/DataTableQuery.h"
#include "wunthshin/Interfaces/Taker/Taker.h"

// Add default functionality here for any ICommonPawn functions that are not pure virtual.

AwunthshinPlayerState* ICommonPawn::GetPlayerState() const
{
    return Cast<APawn>(this)->GetPlayerState<AwunthshinPlayerState>();
}

void ICommonPawn::UpdatePawnFromDataTable(const FCharacterTableRow* InData)
{
    if (InData->SkeletalMesh)
    {
        GetSkeletalMeshComponent()->SetSkeletalMesh(InData->SkeletalMesh);
    }

    if (InData->AnimInstance)
    {
        GetSkeletalMeshComponent()->SetAnimInstanceClass(InData->AnimInstance);
    }

    if (InData->bCustomCapsuleSize)
    {
        GetCapsuleComponent()->SetCapsuleSize(InData->Radius, InData->HalfHeight);
        GetSkeletalMeshComponent()->SetRelativeLocation({ 0.f, 0.f, -InData->HalfHeight });
    }
    else
    {
        if (UCapsuleComponent* CapsuleComponent = GetCapsuleComponent())
        {
            const FBoxSphereBounds& MeshBounds = GetSkeletalMeshComponent()->GetLocalBounds();
            CapsuleComponent->SetCapsuleSize(MeshBounds.BoxExtent.X, MeshBounds.BoxExtent.Z);
        }
    }

    if (!InData->MeshOffset.Equals(FTransform::Identity))
    {
        GetSkeletalMeshComponent()->SetRelativeTransform(InData->MeshOffset);
    }

    if (!InData->HitMontages.IsEmpty())
    {
        SetHitMontages(InData->HitMontages);
    }
    
    if (InData->bHasDefaultWeapon)
    {
        if (APawn* PawnCasting = Cast<APawn>(this);
            PawnCasting && PawnCasting->GetWorld()->IsGameWorld())
        {
            // todo: 무기가 두번 스폰됨 (여기서 한번, ChildActorComponent에서 한번)
            AA_WSWeapon* SpawnedWeapon = PawnCasting->GetWorld()->SpawnActorDeferred<AA_WSWeapon>
            (
                AA_WSWeapon::StaticClass(),
                FTransform::Identity,
                PawnCasting,
                PawnCasting,
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn
            );
            SpawnedWeapon->SetAssetName(InData->Weapon.RowName);
            SpawnedWeapon->FinishSpawning(FTransform::Identity);

            if (const UC_WSPickUp* PickupComponent = SpawnedWeapon->GetComponentByClass<UC_WSPickUp>())
            {
                PickupComponent->OnPickUp.Broadcast(PawnCasting);
            }
        }
    }

    if (!InData->Skill.IsNull())
    {
        GetSkillComponent()->SetCharacterSkill(FSkillRowHandle(InData->Skill));
    }

    IDataTableFetcher* Casting = Cast<IDataTableFetcher>(this);
    check(Casting);

    if (USubsystem* Subsystem = Casting->GetSubsystem())
    {
        const IDataTableQuery* TableQuery = Cast<IDataTableQuery>(Subsystem);
        check(TableQuery);

        if (const FCharacterStats* Stats = TableQuery->GetRowValue<FCharacterStats>(InData->Stats.RowName))
        {
            if (UStatsComponent* StatsComponent = GetStatsComponent())
            {
                StatsComponent->InitializeStats(*Stats);   
            }
        }
    }
}

