#include "../Public/login.h"
#include "../../Data/Public/user.hpp"
#include "../../Public/dbcon.hpp"
#include <boost/uuid.hpp>

#include "../../Public/boost-socket.hpp"

HandlerRegistration<LoginHandler> LoginHandlerRegistration( "login" );

bool LoginHandler::ShouldHandle( EMessageType messageType )
{
    return messageType == EMessageType::Login || messageType == EMessageType::Logout;
}

void LoginHandler::Handle( const size_t index, MessageBase& message )
{
    const Database::Table* user_table   = GlobalScope::GetDatabase().GetTable( "users" );

    switch ( message.GetType() )
    {
        case EMessageType::Login:
        {
            auto& loginMessage = CastTo<EMessageType::Login>( message );

            const auto& replyFailed = []( const size_t to, const uint32_t identifier )
            {
                auto reply = make_vec_unique<LoginStatusMessage>( false, 0, UUID(), identifier  );
                GlobalScope::GetNetwork().send<LoginStatusMessage>( to, std::move( reply ) );
            };

            if ( !check_null_trailing( loginMessage.name ) )
            {
                replyFailed( index, loginMessage.messageIdentifier );
                break;
            }

            if ( is_null_container_unseq( loginMessage.hashedPassword ) )
            {
                replyFailed( index, loginMessage.messageIdentifier );
                break;
            }

            if ( const auto id = user_table->Execute<size_t>(
                         &UserProfile::TryLoginVarChar, loginMessage.name, loginMessage.hashedPassword );
                id != 0 )
            {
                static boost::uuids::random_generator uuid_gen;
                const auto& GenerateUUID = []()
                {
                    UUID sessionId{};
                    boost::uuids::uuid uuid{};
                    uuid = uuid_gen();
                    size_t idx = 0;
                    for ( const uint8_t& byte : uuid )
                    {
                        sessionId[idx] = static_cast<std::byte>( byte );
                        idx++;
                    }
                    return sessionId;
                };

                UUID sessionId{};
                {
                    do_lock( m_mtx_, true );
                    do
                    {
                        sessionId = GenerateUUID();
                    }
                    while ( m_login_.contains( sessionId ) );

                    CONSOLE_OUT( __FUNCTION__, "Login Ok for {} with seesion ID {}", id, to_hex_string( sessionId ) )

                    m_login_.insert( { sessionId, id } );
                    do_lock( m_mtx_, false );
                }

                auto reply = make_vec_unique<LoginStatusMessage>( true, id, sessionId, loginMessage.messageIdentifier );
                GlobalScope::GetNetwork().send<LoginStatusMessage>( index, std::move( reply ) );
                break;
            }
            else
            {
                replyFailed( index, loginMessage.messageIdentifier );
                break;
            }

            // uncovered condition
            assert( false );
            break;
        }
        case EMessageType::Logout:
        {
            const auto& logoutMessage = CastTo<EMessageType::Logout>( message );
            
            do_lock( m_mtx_, true );
            if ( !is_null_container_unseq( logoutMessage.sessionId ) && 
                 m_login_.contains( logoutMessage.sessionId ) )
            {
                m_login_.erase( logoutMessage.sessionId );
                GlobalScope::GetNetwork().send<LogoutOKMessage>(
                        index, make_vec_unique<LogoutOKMessage>( true, logoutMessage.sessionId ) );
                do_lock( m_mtx_, false );
                break;
            }
            else
            {
                GlobalScope::GetNetwork().send<LogoutOKMessage>(
                        index, make_vec_unique<LogoutOKMessage>( false, logoutMessage.sessionId ) );
                do_lock( m_mtx_, false );
                break;
            }

            do_lock( m_mtx_, false );
            assert( false );
            break;
        }
        default: break;
    }
}

size_t LoginHandler::GetLoginUser( const UUID& InSessionId ) const
{
    do_lock( m_mtx_, true );

    if (m_login_.contains(InSessionId))
    {
        const auto returnValue = m_login_.at( InSessionId );
        do_lock( m_mtx_, false );
        return returnValue;
    }

    do_lock( m_mtx_, false );
    return -1;
}
