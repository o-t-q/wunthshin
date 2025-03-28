#include "../Public/register.h"
#include <regex>
#include "../../Public/boost-socket.hpp"
#include "../../Data/Public/user.hpp"
#include "../../Data/Public/inventory.hpp"
#include "../../Public/dbcon.hpp"

HandlerRegistration<RegisterHandler> RegisterHandlerRegistration = {};

bool RegisterHandler::ShouldHandle( EMessageType messageType )
{
    return messageType == EMessageType::Register;
}

void RegisterHandler::Handle( const size_t index, MessageBase& message )
{
    static std::regex emailValidation( R"(^[\w-\.]+@([\w-]+\.)+[\w-]{2,4}$)" );
    const auto&       registerMessage = reinterpret_cast<RegisterMessage&>( message );

    const Database::Table* userTable = GlobalScope::GetDatabase().GetTable( "User" );
    const Database::Table* inventoryTable = GlobalScope::GetDatabase().GetTable( "Inventory" );

    UserProfile newProfile;
    newProfile.name           = registerMessage.name;
    newProfile.email          = registerMessage.email;
    newProfile.hashedPassword = registerMessage.hashedPassword;

    bool success  = true;
    auto failCode = ERegistrationFailCode::None;

    if ( userTable->Execute<bool>( &UserProfile::FindName, newProfile.name ) )
    {
        success = false;
        failCode = ERegistrationFailCode::Name;
    }

    if ( std::cmatch matchResult;
        !std::regex_match( newProfile.email.c_str(), matchResult, emailValidation ) && matchResult.size() != 1 )
    {
        success = false;
        failCode = ERegistrationFailCode::Email;
    }

    if ( success && userTable->Execute<bool>( &UserProfile::Insert, newProfile ) )
    {
        CONSOLE_OUT( __FUNCTION__, "Registration of user {} succeded", newProfile.name )
        const auto user_id = userTable->Execute<size_t>( &UserProfile::GetIdentifier, newProfile.name );
        // Possible database error
        assert( inventoryTable->Execute<bool>( &Inventory::Insert, user_id ) );
        CONSOLE_OUT( __FUNCTION__, "Registration of user {} inventory succeded", newProfile.name )
    }

    auto registerReply = std::make_unique<RegisterStatusMessage>( success, failCode );
    GlobalScope::GetNetwork().send<RegisterStatusMessage>( index, std::move( registerReply ) );
}
