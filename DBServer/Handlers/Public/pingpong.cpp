#include "pingpong.h"
#include "../../message.h"
#include "../../utility.hpp"
#include "../../DBServer.h"
#include "../../boost-socket.hpp"

Registration<PingPongHandler> PingPongRegistration = {};

bool PingPongHandler::ShouldHandle( EMessageType messageType )
{
    return messageType == EMessageType::PingPong;
}

void PingPongHandler::Handle( const size_t index, MessageBase& message )
{
    if ( m_pong_waiting_.contains( index ) )
    {
        CONSOLE_OUT( __FUNCTION__, "Received pong" );
        m_pong_waiting_.erase( index );
        return;
    }
    
    CONSOLE_OUT( __FUNCTION__, "Received ping, reply with pong..." );
    std::unique_ptr<PingPongMessage> reply = std::make_unique<PingPongMessage>();
    GlobalScope::GetNetwork().send( index, std::move( reply ) );
}

void PingPongHandler::Ping( const size_t index )
{
    CONSOLE_OUT( __FUNCTION__, "Send the ping..." );
    std::unique_ptr<PingPongMessage> request = std::make_unique<PingPongMessage>();
    GlobalScope::GetNetwork().send( index, std::move( request ) );

    m_pong_waiting_.insert( index );
}
