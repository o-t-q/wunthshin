#include "../Public/character.h"
#include "../Public/login.h"
#include "../../Public/dbcon.hpp"
#include "../../Public/utility.hpp"
#include "../../Data/Public/characterStatus.hpp"

HandlerRegistration<CharacterStatusHandler> CharacterStatusHandlerRegistration( "characterstatus" );

bool CharacterStatusHandler::ShouldHandle( EMessageType messageType )
{
    return messageType == EMessageType::CharacterStatus;
}

void CharacterStatusHandler::Handle( const size_t index, MessageBase& message )
{
    switch (message.GetType())
    {
    case EMessageType::CharacterStatus:
        HandleChangeStatus( index, message );
        break;
    default:
        break;
    }
}

void CharacterStatusHandler::HandleChangeStatus( const size_t Index, MessageBase& message )
{
    CharacterStatusMessage& characterMessage = CastTo<EMessageType::CharacterStatus>( message );
    LoginHandler*           loginHandler = GlobalScope::GetHandler().GetHandler<LoginHandler>( "login" );
    const size_t userId = loginHandler->GetLoginUser( characterMessage.sessionID );

    const Database::Table* characterTable = GlobalScope::GetDatabase().GetTable( "characterstatus" );
    Characterstatus        Status;
    Status.user_id = userId;
    Status.character_id = characterMessage.character_id;
    Status.current_hp = characterMessage.changed_Hp;
    Status.current_exp = characterMessage.changed_Exp;
       

    if ( characterTable->Execute<bool>( &Characterstatus::UpdateValue, Status ) )
    {
        CONSOLE_OUT( __FUNCTION__, "CurrentHP : {}, CurrentEXP : {}", Status.current_hp, Status.current_exp );
    }
    else
    {
        
    }
}
