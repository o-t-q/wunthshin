#include "Data/Effect/EffectRowHandle.h"

uint32 GetTypeHash(const FEffectRowHandle& InDataTableHandle)
{
	return CityHash32(reinterpret_cast<const char*>(&InDataTableHandle), sizeof(InDataTableHandle));
}