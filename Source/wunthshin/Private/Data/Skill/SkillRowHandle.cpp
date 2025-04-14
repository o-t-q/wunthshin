#include "Data/Skill/SkillRowHandle.h"

uint32 GetTypeHash(const FSkillRowHandle& InDataTableHandle)
{
	return CityHash32(reinterpret_cast<const char*>(&InDataTableHandle), sizeof(InDataTableHandle));
}