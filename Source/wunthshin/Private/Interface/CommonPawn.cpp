// Fill out your copyright notice in the Description page of Project Settings.

#include "Interface/CommonPawn.h"
#include "Data/Character/CharacterTableRow.h"
#include "Data/Character/CharacterStats.h"
#include "Component/StatsComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "wunthshinPlayerState.h"
#include "Actor/Item/A_WSWeapon.h"
#include "Component/C_WSPickUp.h"
#include "Component/C_WSSkill.h"
#include "Interface/DataTableFetcher.h"
#include "Interface/DataTableQuery.h"
#include "Interface/Taker.h"
#include "Widget/WG_WSDamageCounter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Add default functionality here for any ICommonPawn functions that are not pure virtual.

const FName ICommonPawn::InventoryComponentName = "InventoryComponent";

AwunthshinPlayerState* ICommonPawn::GetPlayerState() const
{
    return Cast<APawn>(this)->GetPlayerState<AwunthshinPlayerState>();
}

void ICommonPawn::UpdatePawnFromDataTable(const FCharacterTableRow* InData)
{
    if (InData->SkeletalMesh)
    {
        GetSkeletalMeshComponent()->SetSkeletalMesh(InData->SkeletalMesh);
        
        if (APawn* PawnCasting = Cast<APawn>(this);
            PawnCasting && (PawnCasting->GetWorld()->IsGameWorld() || PawnCasting->GetWorld()->IsPlayInEditor()))
        {
            GetDamageCounters()->Initialize(PawnCasting, {}, GetSkeletalMeshComponent());
        }
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

