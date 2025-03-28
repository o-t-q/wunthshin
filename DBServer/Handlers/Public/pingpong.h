#pragma once
#include <set>

#include "../../Public/message.h"
#include "../../Public/message-handler.h"

struct PingPongHandler : HandlerImplementation
{
    bool ShouldHandle( EMessageType messageType ) override;
    void Handle( const size_t index, MessageBase& message ) override;
    
    void Ping( const size_t index );

    std::set<size_t> m_pong_waiting_;
};
