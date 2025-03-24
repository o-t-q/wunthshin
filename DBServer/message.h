#pragma once
#include <type_traits>

enum class EMessageType
{
	PingPong,
};

#pragma pack( push, 1 )
template <EMessageType MessageType>
struct MessageT
{
    EMessageType GetType() const
    {
        return MessageType;
    }

private:
    EMessageType messageType = MessageType;
};

struct PingPongMessage : MessageT<EMessageType::PingPong> {};
#pragma pack( pop )
