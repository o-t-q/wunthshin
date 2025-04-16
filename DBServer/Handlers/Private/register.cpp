#include "../Public/register.h"
#include <regex>
#include "../../Public/boost-socket.hpp"
#include "../../Data/Public/user.hpp"
#include "../../Data/Public/inventory.hpp"
#include "../../Public/dbcon.hpp"

HandlerRegistration<RegisterHandler> RegisterHandlerRegistration( "register" );

bool RegisterHandler::ShouldHandle( EMessageType messageType )
{
    return messageType == EMessageType::Register;
}

void RegisterHandler::Handle( const size_t index, MessageBase& message )
{
    static std::regex emailValidation( R"(^[\w-\.]+@{1}([\w-]+\.)+[\w-]{2,4}$)" );
    static std::regex idValidation( R"(^[a-zA-Z0-9]+$)") ;
    const auto&       registerMessage = CastTo<EMessageType::Register>( message );

    const Database::Table* userTable = GlobalScope::GetDatabase().GetTable( "users" );
    const Database::Table* inventoryTable = GlobalScope::GetDatabase().GetTable( "inventory" );

    bool success  = true;
    auto failCode = ERegistrationFailCode::None;
    decltype( registerMessage.name )::const_iterator nameNullTrailing;
    decltype( registerMessage.email )::const_iterator emailNullTrailing;

    if ( !check_null_trailing(registerMessage.name, nameNullTrailing ) )
    {
        CONSOLE_OUT( __FUNCTION__, "Empty name given" )
        success  = false;
        failCode = ERegistrationFailCode::Name;
    }

    if ( !check_null_trailing(registerMessage.email, emailNullTrailing) )
    {
        CONSOLE_OUT( __FUNCTION__, "Empty email given" )
        success  = false;
        failCode = ERegistrationFailCode::Email;
    }

    if ( success )
    {
        UserProfile newProfile{};
        std::copy( registerMessage.name.begin(), nameNullTrailing, newProfile.name.begin() );
        std::copy( registerMessage.email.begin(), emailNullTrailing, newProfile.email.begin() );
        newProfile.hashedPassword = registerMessage.hashedPassword;

        std::string_view nameStringify( newProfile.name.data() );
        std::string_view emailStringify( newProfile.email.data() );

        if ( userTable->Execute<bool>( &UserProfile::FindName, nameStringify ) )
        {
            CONSOLE_OUT( __FUNCTION__, "Registration failed due to the duplicated name" )
            success  = false;
            failCode = ERegistrationFailCode::Name;
        }

        if ( std::cmatch matchResult;
             !std::regex_match( nameStringify.data(), matchResult, idValidation ) || matchResult.empty() )
        {
            CONSOLE_OUT( __FUNCTION__, "Registration failed due to the name rule" )
            success  = false;
            failCode = ERegistrationFailCode::Name;
        }

        if ( std::cmatch matchResult;
             !std::regex_match( emailStringify.data(), matchResult, emailValidation ) || matchResult.empty() )
        {
            CONSOLE_OUT( __FUNCTION__, "Registration failed due to the email rule" )
            success  = false;
            failCode = ERegistrationFailCode::Email;
        }

        if ( success )
        {
            if (userTable->Execute<bool>(&UserProfile::Insert, newProfile))
            {
                CONSOLE_OUT( __FUNCTION__, "Registration of user {} succeded", nameStringify )
                const auto user_id = userTable->Execute<size_t>( &UserProfile::GetIdentifier, nameStringify );
                // Possible database error
                if (inventoryTable->Execute<bool>(&Inventory::Insert, user_id))
                {
                    CONSOLE_OUT( __FUNCTION__, "Registration of user {} inventory succeded", nameStringify )
                }
            }
            else
            {
                assert( false );
                success = false;
            }            
        }
    }

    auto registerReply = make_vec_unique<RegisterStatusMessage>( success, failCode );
    GlobalScope::GetNetwork().send<RegisterStatusMessage>( index, std::move( registerReply ) );
}
