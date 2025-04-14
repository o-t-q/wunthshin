#include "Subsystem/EventTicket/ElementTicket/ElementReactTicket.h"

#include "Data/Element/O_WSElementReactor.h"
#include "Data/Element/ElementTracking.h"
#include "Interface/ElementTracked.h"
#include "Subsystem/ElementSubsystem.h"
#include "Subsystem/WorldStatusSubsystem.h"

DEFINE_LOG_CATEGORY(LogElementTicket);

void FElementReactTicket::Execute(UWorld* InWorld)
{
	// 빈 요청
	if (ElementHandle.Handle.IsNull())
	{
		SetDisposed();
		return;
	}
	
	UE_LOG(LogElementTicket, Log, TEXT("Applying the element %s to %s by %s"), *ElementHandle.Handle.RowName.ToString(), *TargetActor->GetName(), *Instigator->GetName());

	if (UWorldStatusSubsystem* WorldStatusSubsystem = InWorld->GetSubsystem<UWorldStatusSubsystem>())
	{
		if (WorldStatusSubsystem->IsElementalTracking(TargetActor)) 
		{
			FElementTrackingMap& Map = WorldStatusSubsystem->GetElementTracking(TargetActor);
		
			// 중복된 원소효과는 해당 원소의 타이머를 갱신
			if (Map.Contains(ElementHandle))
			{
				UE_LOG(LogElementTicket, Log, TEXT("Element %s exists, extending the timer"), *ElementHandle.Handle.RowName.ToString());
				Map.ResetTimer(InWorld, ElementHandle);
			}
		
			Map.Add(InWorld, Instigator, ElementHandle);

			if (IElementTracked* Interface = Cast<IElementTracked>(TargetActor))
			{
				Interface->OnElementApplied.Broadcast(ElementHandle);	
			}

			if (const UElementSubsystem* ElementSubsystem = InWorld->GetGameInstance()->GetSubsystem<UElementSubsystem>();
				ElementSubsystem && Map.IsFull())
			{
				const FElementReactionPair& Pair = Map.ExtractElement();
				UO_WSElementReactor* Reactor = ElementSubsystem->GetReactor(Pair.Elements[0], Pair.Elements[1]);

				if (!Reactor)
				{
					// 매칭되는 요소의 반응이 없는 경우
					ensure(false);
					SetDisposed();
					return;
				}
			
				if (Pair.Instigators.Num() != 2) 
				{
					// element를 적용한 대상이 두명이 아닌 경우 
					// (둘 다 같은 instigator는 예외, 같은 Instigator가 다른 속성 두개를 부여할 수 있음)
					ensure(false);
					SetDisposed();
					return;
				}
			
				AActor* OtherInstigator = Pair.Instigators[0] == Instigator ? Pair.Instigators[1] : Pair.Instigators[0];
				UE_LOG(LogElementTicket, Log, TEXT("%s effect triggered by %s and %s!"), *Reactor->GetName(), *Instigator->GetName(), *OtherInstigator->GetName());
				Reactor->React(Instigator, OtherInstigator, TargetActor);

				TSharedPtr<FElementReactRevokeTicket> ReactFinishTicket = MakeShared<FElementReactRevokeTicket>();
				ReactFinishTicket->TargetActor = TargetActor;
				WorldStatusSubsystem->PushTicket(ReactFinishTicket);
			}
		}
		else
		{
			// 처음 부여된 상태이면 효과가 발생할 수 없음 (2개의 다른 조합으로 효과 발생)
			FElementTrackingMap& NewMap = WorldStatusSubsystem->AddElementTracking(TargetActor);
			NewMap.Add(InWorld, Instigator, ElementHandle);
		}
	}

	SetDisposed();
}

void FElementReactRevokeTicket::Execute(UWorld* InWorld)
{
	// 지금까지 추적하던 원소 상태 초기화
	if (UWorldStatusSubsystem* WorldStatusSubsystem = InWorld->GetSubsystem<UWorldStatusSubsystem>())
	{
		if (WorldStatusSubsystem->IsElementalTracking(TargetActor))
		{
			UE_LOG(LogElementTicket, Log, TEXT("Remove element tracking of %s"), *TargetActor->GetName())
			WorldStatusSubsystem->RemoveElementTracking(TargetActor);
		}
	}

	SetDisposed();
}
