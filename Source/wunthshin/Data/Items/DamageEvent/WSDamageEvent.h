#pragma once
#include "Engine/DamageEvents.h"

class ICommonPawn;

struct FWSDamageEvent : public FDamageEvent
{
	FGuid AttackID;

	FWSDamageEvent() {}
	
	explicit FWSDamageEvent(const FDamageEvent& InDamageEvent, const FGuid& InGuid)
		: FDamageEvent(InDamageEvent), AttackID(InGuid)
	{
	}

	void SetDamageEvent(const FGuid& InAttackID)
	{
		// 무기 고유번호가 생성된 값이 아닌 경우
		check(InAttackID.IsValid());
		AttackID = InAttackID;
	}
	bool IsValid() const { return AttackID.IsValid(); }
	bool IsFirstHit(const ICommonPawn* InPawn) const;
	void SetFirstHit(const ICommonPawn* InPawn) const;
};
