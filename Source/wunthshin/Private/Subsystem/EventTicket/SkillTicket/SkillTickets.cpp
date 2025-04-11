#include "Subsystem/EventTicket/SkillTicket/SkillTickets.h"

#include "Data/Skill/O_WSBaseSkill.h"
#include "Data/Skill/SkillTableRow.h"
#include "Interface/ElementTracked.h"
#include "Subsystem/SkillSubsystem.h"
#include "Subsystem/WorldStatusSubsystem.h"

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
				if (WorldStatusSubsystem->IsLevelSequencePlaying())
				{
					SetDisposed();
					return;
				}
				
				UE_LOG(LogSkillSubsystem, Log, TEXT("Skill level sequence found, playing: %s"), *Skill->Parameter.CastingSequence->GetName());

				WorldStatusSubsystem->SetSkillVictimPawn(Cast<APawn>(TargetActor));
				WorldStatusSubsystem->FreezeSpawnedNPCsBT();
				WorldStatusSubsystem->PlayLevelSequence(Skill->Parameter.CastingSequence, false);
			}

			const TSharedPtr<FSkillRevokeTicket> SkillEndTicket = MakeShared<FSkillRevokeTicket>();
			SkillEndTicket->Instigator = Instigator;
			SkillEndTicket->SkillProcessor = SkillProcessor;
			SkillEndTicket->SkillHandle = SkillHandle;
			SkillEndTicket->TargetActor = TargetActor;
			SkillEndTicket->TargetLocation = TargetLocation;
			
			WorldStatusSubsystem->PushTicket(SkillEndTicket);
		}
	}
	
	SetDisposed();
}

void FSkillRevokeTicket::Execute(UWorld* InWorld)
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
	
	SetDisposed();
}
