// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FirstHitAttNotify.generated.h"

/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UFirstHitAttNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UFirstHitAttNotify();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
