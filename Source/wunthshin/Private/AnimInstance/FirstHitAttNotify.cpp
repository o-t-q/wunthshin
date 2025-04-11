// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/FirstHitAttNotify.h"

UFirstHitAttNotify::UFirstHitAttNotify()
{
//#if WITH_EDITIONLY_DATA
//	NotifyColor = FColor(123, 123, 123, 123);
//#endif
}

void UFirstHitAttNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	////FVector* Location = &MeshComp->GetOwner()->GetActorLocation();
	//if (!Location) { return; }
	////공격시 케릭터 위치 이동 (완전 이상함 나중에 수정 필요, 이거 틀린거 같음);
	//Location->X = Location->X + 100;



}
