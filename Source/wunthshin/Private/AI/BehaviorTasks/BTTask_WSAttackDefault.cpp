// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTask/BTTask_WSAttackDefault.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Component/C_WSWeapon.h"
#include "Interface/CommonPawn.h"
#include "Subsystem/WorldStatusSubsystem.h"

void UBTTask_WSAttackDefault::CheckIfAttackByNode(FGuid InAttackID)
{
	if (AttacksByThisNode.Contains(InAttackID))
	{
		// 추적중인 공격의 종료가 선언되면 태스크 성공 반환
		if (UBehaviorTreeComponent* BehaviorTreeComponent = AttacksByThisNode[InAttackID])
		{
			FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
		}

		AttacksByThisNode.Remove(InAttackID);
	}
}

EBTNodeResult::Type UBTTask_WSAttackDefault::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type ReturnValue = EBTNodeResult::Failed;

	if (const ICommonPawn* Pawn = Cast<ICommonPawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor")))
	{
		if (UWorldStatusSubsystem* WorldStatusSubsystem = GetWorld()->GetSubsystem<UWorldStatusSubsystem>())
		{
			if (!WorldStatusSubsystem->OnWeaponAttackEnded.Contains(this, "CheckIfAttackByNode"))
			{
				WorldStatusSubsystem->OnWeaponAttackEnded.AddUniqueDynamic(this, &UBTTask_WSAttackDefault::CheckIfAttackByNode);
			}
		}
		
		// 공격자 -> 무기 -> 무기 컴포넌트
		if (const AActor* Actor = Pawn->GetRightHandComponent()->GetChildActor())
		{
			if (UC_WSWeapon* WeaponComponent = Actor->GetComponentByClass<UC_WSWeapon>())
			{
				// 공격중인지 체크
				if (WeaponComponent->IsAttackInProgress())
				{
					ReturnValue = EBTNodeResult::Failed;
				}
				else
				{
					if (WeaponComponent->AttackDefault())
					{
						ReturnValue = EBTNodeResult::InProgress;

						// 공격의 고유아이디를 받아오고 노드로 발생한 공격의 상태를 추적
						const FGuid AttackID = GetWorld()->GetSubsystem<UWorldStatusSubsystem>()->GetCurrentAttackID(WeaponComponent);
						AttacksByThisNode.Add(AttackID);
						AttacksByThisNode[AttackID] = &OwnerComp;
					}
				}
			}
		}
	}
	
	return ReturnValue;
}

UBTTask_WSAttackDefault::UBTTask_WSAttackDefault()
{
	NodeName = "Do Default Attack";
}

EBTNodeResult::Type UBTTask_WSAttackDefault::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
