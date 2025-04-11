#include "Subsystem/EventTicket/ItemTicket/ItemTicket.h"

#include "Data/Effect/O_WSBaseEffect.h"
#include "Data/Effect/EffectParameter.h"
#include "Data/Item/SG_WSItemMetadata.h"


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
