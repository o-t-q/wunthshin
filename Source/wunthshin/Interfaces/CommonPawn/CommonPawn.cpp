// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonPawn.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"
#include "wunthshin/Data/Characters/CharacterStats/CharacterStats.h"
#include "wunthshin/Components/Stats/StatsComponent.h"
#include "Components/CapsuleComponent.h"
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"

// Add default functionality here for any ICommonPawn functions that are not pure virtual.

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
            CapsuleComponent->SetCapsuleSize(MeshBounds.SphereRadius, MeshBounds.BoxExtent.Z);
        }
    }

    if (!InData->HitMontages.IsEmpty())
    {
        SetHitMontages(InData->HitMontages);
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
