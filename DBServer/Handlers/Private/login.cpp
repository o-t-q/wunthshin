#include "../Public/login.h"
#include "../../Data/Public/user.hpp"
#include "../../Public/dbcon.hpp"
#include <boost/uuid.hpp>

#include "../../Public/boost-socket.hpp"

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

            const auto& replyFailed = []( const size_t to )
            {
                auto reply          = make_vec_unqiue<LoginStatusMessage>( UUID() );
                reply->success = false;
                GlobalScope::GetNetwork().send<LoginStatusMessage>( to, std::move( reply ) );
            };

            if ( !check_null_trailing(loginMessage.name ) )
            {
                replyFailed( index );
                break;
            }

            if ( is_null_container_unseq( loginMessage.hashedPassword ) )
            {
                replyFailed( index );
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
                do
                {
                    sessionId = GenerateUUID();
                } while ( m_login_.contains( sessionId ) );

                CONSOLE_OUT(__FUNCTION__, "Login Ok for {} with seesion ID {}", id, to_hex_string( sessionId ))

                m_login_.insert( { sessionId, id } );
                auto reply     = make_vec_unqiue<LoginStatusMessage>( std::move( sessionId ) );
                reply->success = true;
                GlobalScope::GetNetwork().send<LoginStatusMessage>( index, std::move( reply ) );
                break;
            }
            else
            {
                replyFailed( index );
                break;
            }

            // uncovered condition
            assert( false );
            break;
        }
        case EMessageType::Logout:
        {
            const auto& logoutMessage = reinterpret_cast<LogoutMessage&>( message );
            
            if ( !is_null_container_unseq( logoutMessage.sessionId ) && 
                 m_login_.contains( logoutMessage.sessionId ) )
            {
                m_login_.erase( logoutMessage.sessionId );
                GlobalScope::GetNetwork().send<LogoutOKMessage>( index, make_vec_unqiue<LogoutOKMessage>( true ) );
                break;
            }
            else
            {
                GlobalScope::GetNetwork().send<LogoutOKMessage>( index, make_vec_unqiue<LogoutOKMessage>( false ) );
                break;
            }

            assert( false );
            break;
        }
        default: break;
    }
}
