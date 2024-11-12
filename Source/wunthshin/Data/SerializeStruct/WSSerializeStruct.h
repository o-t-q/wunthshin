#pragma once
#include "wunthshin/Data/Archive/WSArchive.h"

#include "WSSerializeStruct.generated.h"

USTRUCT()
struct FWSSerializeStruct
{
	GENERATED_BODY()
	
public:
	virtual ~FWSSerializeStruct() = default;
	virtual void Serialize(FWSArchive& Ar) PURE_VIRTUAL(FSerializeStruct::Seralize, );
};
