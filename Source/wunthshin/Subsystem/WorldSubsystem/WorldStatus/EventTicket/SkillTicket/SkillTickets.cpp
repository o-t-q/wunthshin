#include "SkillTickets.h"

#include "wunthshin/Data/Skills/O_WSBaseSkill.h"
#include "wunthshin/Data/Skills/SkillTableRow/SkillTableRow.h"
#include "wunthshin/Interfaces/ElementTracked/ElementTracked.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Skill/SkillSubsystem.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"

void FSkillStartTicket::Execute(UWorld* InWorld)
{
	if (const FSkillTableRow* Skill = SkillHandle.Handle.GetRow<FSkillTableRow>(TEXT("")))
	{
		UO_WSBaseSkill* SkillProcessor = InWorld->GetGameInstance()->GetSubsystem<USkillSubsystem>()->GetSkillProcessor(Skill->SkillProcessor);
		// 미리 스킬 처리기가 생성되지 않은 경우
		check(SkillProcessor);
		
		if (UWorldStatusSubsystem* WorldStatusSubsystem = InWorld->GetSubsystem<UWorldStatusSubsystem>())
		{
			if (Skill->Parameter.CastingSequence)
			{
				UE_LOG(LogSkillSubsystem, Log, TEXT("Skill level sequence found, playing: %s"), *Skill->Parameter.CastingSequence->GetName());
				if (WorldStatusSubsystem->IsLevelSequencePlaying())
				{
					return;
				}

				WorldStatusSubsystem->SetSkillVictimPawn(Cast<APawn>(Instigator));
				WorldStatusSubsystem->FreezeSpawnedNPCsBT();
				WorldStatusSubsystem->PlayLevelSequence(Skill->Parameter.CastingSequence);
			}

			TSharedPtr<FSkillEndTicket> SkillEndTicket = MakeShared<FSkillEndTicket>();
			SkillEndTicket->Instigator = Instigator;
			SkillEndTicket->SkillProcessor = SkillProcessor;
			SkillEndTicket->SkillHandle = SkillHandle;
			SkillEndTicket->TargetActor = TargetActor;
			SkillEndTicket->TargetLocation;
			
			WorldStatusSubsystem->PushTicket(SkillEndTicket);
		}
	}
}

void FSkillEndTicket::Execute(UWorld* InWorld)
{
	if (const FSkillTableRow* Skill = SkillHandle.Handle.GetRow<FSkillTableRow>(TEXT("")))
	{
		if (AActor* InstigatorActor = Cast<AActor>(Instigator))
		{
			UE_LOG(LogSkillSubsystem, Log, TEXT("Take effect of skill! %s and %s -> %s, %s"), *SkillProcessor->GetName(), *InstigatorActor->GetName(), *TargetLocation.ToString(), *TargetActor->GetName());

			SkillProcessor->DoSkill(Skill->Parameter, Instigator, TargetLocation, TargetActor);
			if (IElementTracked* ElementTracked = Cast<IElementTracked>(TargetActor))
			{
				if (!Skill->Element.IsNull())
				{
					ElementTracked->ApplyElement(InstigatorActor, FElementRowHandle{Skill->Element});
				}
			}
				
			InstigatorActor->GetWorld()->GetSubsystem<UWorldStatusSubsystem>()->UnfreezeSpawnedNPCsBT();
		}
	}
}
