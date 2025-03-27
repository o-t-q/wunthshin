#include "../Public/login.h"
#include "../../Data/Public/user.hpp"
#include "../../dbcon.hpp"
#include <boost/uuid.hpp>

#include "../../boost-socket.hpp"

HandlerRegistration<LoginHandler> LoginHandlerRegistration = {};

bool LoginHandler::ShouldHandle( EMessageType messageType )
{
    return messageType == EMessageType::Login || messageType == EMessageType::Logout;
}

void LoginHandler::Handle( const size_t index, MessageBase& message )
{
    const Database::Table* user_table   = GlobalScope::GetDatabase().GetTable( "User" );

    switch ( message.GetType() )
    {
        case EMessageType::Login:
        {
            auto& loginMessage = reinterpret_cast<LoginMessage&>( message );
            if ( const auto id = user_table->Execute<size_t>(
                    std::bind_front( &UserProfile::TryLogin, loginMessage.name, loginMessage.hashedPassword ) );
                id != 0 )
            {
                static boost::uuids::random_generator uuid_gen;
                boost::uuids::uuid uuid = uuid_gen();
                UUID sessionId{};

                size_t idx = 0;
                for ( const uint8_t& byte : uuid )
                {
                    sessionId[idx] = static_cast<std::byte>( byte );
                    idx++;
                }

                CONSOLE_OUT(__FUNCTION__, "Login Ok for {} with seesion ID {}", id, to_hex_string( sessionId ))

                m_login_.insert( { sessionId, id } );
                auto reply = std::make_unique<LoginOKMessage>( std::move( sessionId ) );
                GlobalScope::GetNetwork().send<LoginOKMessage>( index, std::move( reply ) ); 
            }
            break;
        }
        case EMessageType::Logout:
        {
            const auto& logoutMessage = reinterpret_cast<LogoutMessage&>( message );
            
            if ( m_login_.contains( logoutMessage.sessionId ) )
            {
                m_login_.erase( logoutMessage.sessionId );
                GlobalScope::GetNetwork().send<LogoutOKMessage>( index, std::make_unique<LogoutOKMessage>( true ) );
            }
            else
            {
                GlobalScope::GetNetwork().send<LogoutOKMessage>( index, std::make_unique<LogoutOKMessage>( false ) );
            }
        }
        default: break;
    }
}
