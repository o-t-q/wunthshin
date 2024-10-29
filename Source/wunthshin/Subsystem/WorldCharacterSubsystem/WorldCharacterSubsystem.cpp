// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldCharacterSubsystem.h"

#include "wunthshin/Actors/Item/A_WSItem.h"
#include "wunthshin/Actors/AA_WSCharacter.h"
#include "Engine/OverlapResult.h"
#include "Components/WidgetComponent.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"

void UWorldCharacterSubsystem::Tick(float InDeltaTime)
{
    ItemsNearbyCharacter.Empty();

	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()) 
	{
		if (const AA_WSCharacter* PlayerCharacter = Cast<AA_WSCharacter>(PlayerController->GetCharacter()))
		{
            TArray<FOverlapResult> Overlaps;
            FCollisionQueryParams QueryParams(NAME_None, false, PlayerCharacter);

            // 손에 있는 무기는 주변 아이템 대상에서 제외
            if (const AActor* ChildWeaponActor = PlayerCharacter->GetRightHandWeapon()->GetChildActor())
            {
                QueryParams.AddIgnoredActor(ChildWeaponActor);
            }

            const FVector& PlayerLocation = PlayerCharacter->GetActorLocation();

            // 반환 값은 blocking일때 참을 반환하나, overlap으로 trace channel을 쓰기 때문에 무시함
            GetWorld()->OverlapMultiByChannel
            (
                Overlaps,
                PlayerLocation,
                FQuat::Identity,
                ECC_GameTraceChannel2, // ItemCheck Channel
                FCollisionShape::MakeSphere(100.f)
            );

            if (!Overlaps.IsEmpty())
            {
                // 소유권자가 있는 아이템은 제외
                Overlaps.RemoveAll(
                    [](const FOverlapResult& InResult) 
                    {
                        return InResult.GetActor()->GetComponentByClass<UC_WSPickUp>()->IsTaken();
                    });

                // Overlap된 물체들을 거리 순서대로 정렬한다
                Overlaps.Sort
                    ([&PlayerLocation](const FOverlapResult& Left, const FOverlapResult& Right)
                    {
                        return FVector::Distance(Left.GetActor()->GetActorLocation(), PlayerLocation) >
                            FVector::Distance(Right.GetActor()->GetActorLocation(), PlayerLocation);
                    });

                for (FOverlapResult& Result : Overlaps)
                {
                    ItemsNearbyCharacter.Add(Result.GetActor());
                }

                Cast<AA_WSItem>(GetNearestItem())->GetItemNotifyWidget()->GetWidget()->SetVisibility(ESlateVisibility::Visible);
            }
		}
	}
}
