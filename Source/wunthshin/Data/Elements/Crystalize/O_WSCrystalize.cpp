// Fill out your copyright notice in the Description page of Project Settings.


#include "O_WSCrystalize.h"

#include "wunthshin/Components/Shield/C_WSShield.h"

void UO_WSCrystalize::React(AActor* InReactionInstigator, AActor* InReactionAssist, AActor* InReactionOrigin)
{
	if (UC_WSShield* ShieldComponent = InReactionInstigator->GetComponentByClass<UC_WSShield>())
	{
		// todo: 쉴드 시간 조정 (레벨, 아이템 비례?)
        ShieldComponent->SetShieldTime(5.f);
        ShieldComponent->SetActive(true);
	}

	if (UC_WSShield* ShieldComponent = InReactionAssist->GetComponentByClass<UC_WSShield>())
	{
		// todo: 쉴드 시간 조정 (레벨, 아이템 비례?)
		ShieldComponent->SetShieldTime(5.f);
		ShieldComponent->SetActive(true);
	}
}
