#include "../Public/pingpong.h"
#include "../../Public/message.h"
#include "../../Public/utility.hpp"
#include "../../Public/DBServer.h"
#include "../../Public/boost-socket.hpp"

HandlerRegistration<PingPongHandler> PingPongRegistration = {};

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
    accessor reply = make_vec_unique<PingPongMessage>();
    GlobalScope::GetNetwork().send( index, std::move( reply ) );
}

void PingPongHandler::Ping( const size_t index )
{
    CONSOLE_OUT( __FUNCTION__, "Send the ping..." );
    accessor request = make_vec_unique<PingPongMessage>();
    GlobalScope::GetNetwork().send( index, std::move( request ) );

    m_pong_waiting_.insert( index );
}
