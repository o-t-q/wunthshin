#include "WSArchive.h"

#include "wunthshin/Data/SerializeStruct/WSSerializeStruct.h"

void FWSArchive::SerializeObject(FWSSerializeStruct& InObject)
{
	InObject.Serialize(*this);
}

FWSArchive& FWSArchive::operator<<(FWSSerializeStruct& InObject)
{
	SerializeObject(InObject);
	return *this;
}
