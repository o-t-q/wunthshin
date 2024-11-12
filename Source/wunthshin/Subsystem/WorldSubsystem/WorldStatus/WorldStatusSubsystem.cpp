// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldStatusSubsystem.h"

#include "LevelSequencePlayer.h"

#include "BehaviorTree/BehaviorTreeComponent.h"

#include "wunthshin/Actors/Item/A_WSItem.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Data/Effects/O_WSBaseEffect.h"
#include "Engine/OverlapResult.h"
#include "Components/WidgetComponent.h"
#include "EventTicket/EventTicket.h"
#include "EventTicket/ItemTicket/ItemTicket.h"

#include "wunthshin/Actors/Pawns/Character/AA_WSCharacter.h"
#include "wunthshin/Actors/Pawns/NPC/A_WSNPCPawn.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"

void UWorldStatusSubsystem::PushTicket_Internal(TSharedPtr<FEventTicket> InTicket)
{
    EventQueue.Push(InTicket);
}

UWorldStatusSubsystem::UWorldStatusSubsystem()
    : CurrentLevelSequence(nullptr), LevelSequenceActor(nullptr), SkillVictimPawn(nullptr)
{
}

void UWorldStatusSubsystem::Tick(float InDeltaTime)
{
    ItemsNearbyCharacter.Empty();

	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController()) 
	{
		if (const AA_WSCharacter* PlayerCharacter = Cast<AA_WSCharacter>(PlayerController->GetCharacter()))
		{
            TArray<FOverlapResult> Overlaps;
            FCollisionQueryParams QueryParams(NAME_None, false, PlayerCharacter);

            // 손에 있는 무기는 주변 아이템 대상에서 제외
            if (const AActor* ChildWeaponActor = PlayerCharacter->GetRightHandComponent()->GetChildActor())
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

                // 가장 가까운 아이템의 줍기 알림을 띄움 (Visibility Hidden이 되면 위젯 Tick이 돌지 않음)
                Cast<AA_WSItem>(GetNearestItem())->GetItemNotifyWidget()->GetWidget()->SetVisibility(ESlateVisibility::Visible);
            }
		}
	}

    // 레벨 시퀀스가 시작되어 있지 않다면 지속적으로 이벤트 처리
    while (!EventQueue.IsEmpty()) 
    {
        if (GetCurrentLevelSequence())
        {
            break;
        }

        if (TSharedPtr<FEventTicket> Ticket = EventQueue.Pop();
            Ticket->IsValid()) 
        {
            Ticket->Execute(GetWorld());
            
            FTimerDelegate Delegate;
            FTimerManagerTimerParameters Params;
            Params.bLoop = false;
            Params.FirstDelay = -1.f;
            Params.bMaxOncePerFrame = false;

            Delegate.BindUObject
            (
                this,
                &UWorldStatusSubsystem::PushTicket_Internal,
                Ticket
            );

            GetWorld()->GetTimerManager().SetTimer
            (
                Ticket->GetTimerHandle(),
                Delegate,
                Ticket->GetRate(),
                Params
            );
        }
    }
}

void UWorldStatusSubsystem::PlayLevelSequence(ULevelSequence* InSequence, const TFunction<void()>& OnEndedFunction)
{
    FMovieSceneSequencePlaybackSettings PlaybackSettings;
    PlaybackSettings.bAutoPlay = true;
    PlaybackSettings.LoopCount.Value = 0;
    PlaybackSettings.bHideHud = true;
    PlaybackSettings.bDisableLookAtInput = true;
    PlaybackSettings.bDisableMovementInput = true;
				
    CurrentLevelSequence = ULevelSequencePlayer::CreateLevelSequencePlayer
    (
        GetWorld(),
        InSequence,
        PlaybackSettings,
        LevelSequenceActor
    );
    
    OnLevelSequenceEnded = OnEndedFunction;
    CurrentLevelSequence->OnFinished.AddUniqueDynamic(this, &UWorldStatusSubsystem::ClearLevelSequence);
}

void UWorldStatusSubsystem::FreezeSpawnedNPCsBT()
{
    for (const AA_WSNPCPawn* Pawn : NPCPawns)
    {
        Pawn->GetController()->GetComponentByClass<UBehaviorTreeComponent>()->PauseLogic("By world status subsystem");
    }
}

void UWorldStatusSubsystem::UnfreezeSpawnedNPCsBT()
{
    for (const AA_WSNPCPawn* Pawn : NPCPawns)
    {
        Pawn->GetController()->GetComponentByClass<UBehaviorTreeComponent>()->ResumeLogic("By world status subsystem");
    }
}

void UWorldStatusSubsystem::PushTicket(TWeakPtr<FEventTicket> Ticket)
{
    if (Ticket.IsValid())
    {
        EventQueue.Add(Ticket.Pin());
    }
}

void UWorldStatusSubsystem::PushTicketScheduled(TWeakPtr<FEventTicket> Ticket, FTimerHandle& InTimerHandle, const float InDuration)
{
    if (Ticket.IsValid())
    {
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUObject(this, &UWorldStatusSubsystem::PushTicket_Internal, Ticket.Pin());
        
        GetWorld()->GetTimerManager().SetTimer
        (
            InTimerHandle,
            TimerDelegate,
            InDuration,
            false,
            -1
        );
    }
}

void UWorldStatusSubsystem::PushItem(const USG_WSItemMetadata* InItem, AActor* InInstigator, AActor* InTarget)
{
    if (InItem && InInstigator && InTarget) 
    {
        const TSharedPtr<FItemTicket> ItemTicket = MakeShared<FItemTicket>();

        ItemTicket->Item = InItem;
        ItemTicket->Instigator = InInstigator;
        ItemTicket->Target = InTarget;

        const FEffectParameter& Parameter = InItem->GetItemParameter();
        ItemTicket->SetRate(Parameter.PerTime);

        // 아이템의 최종 시전 횟수 초기화 (아이템에 지정된 시간이 있을 경우)
        if (InItem->GetItemParameter().Duration != 0.f)
        {
            ItemTicket->MaxExecuteCount = Parameter.Duration * Parameter.PerTime;
        }
        
        PushTicket(ItemTicket);
    }
}
