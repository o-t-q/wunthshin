#include "ElementRowHandle.h"

uint32 GetTypeHash(const FElementRowHandle& InDataTableHandle)
{
	return CityHash32(reinterpret_cast<const char*>(&InDataTableHandle), sizeof(InDataTableHandle));
}