#include "ItemTicket.h"

#include "wunthshin/Data/Effects/O_WSBaseEffect.h"
#include "wunthshin/Data/Effects/EffectParameter/EffectParameter.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"


void FItemTicket::Execute(UWorld* InWorld)
{
	ExecuteCount++;
    
	const UO_WSBaseEffect*  Effect     = Item->GetItemEffect(InWorld);
	const FEffectParameter& ItemParams = Item->GetItemParameter();
    
	// 아이템 효과 호출
	Effect->Effect(ItemParams, Instigator, Target);
    
	// 아이템 효과 만료시
	if (ExecuteCount >= MaxExecuteCount)
	{
		SetDisposed();
	}
}
