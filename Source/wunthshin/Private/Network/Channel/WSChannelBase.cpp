#include "Network/Channel/WSChannelBase.h"
#include "Network/Subsystem/WSServerSubsystem.h"

UWSServerSubsystem* UWSChannelBase::GetSubsystem()
{
	return Cast<UWSServerSubsystem>(Cast<UNetDriver>(GetOuter())->GetOuter());
}
