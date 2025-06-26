#pragma once
#include "../../Public/message-handler.h"

struct CharacterStatusHandler : HandlerImplementation
{
    bool ShouldHandle( EMessageType messageType ) override;
    void Handle( const size_t index, MessageBase& message ) override;

private:
    void HandleChangeStatus( const size_t Index, MessageBase& message );
};
