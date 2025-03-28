#pragma once

#include "../../message.h"
#include "../../message-handler.h"

struct RegisterHandler : HandlerImplementation
{
    bool ShouldHandle( EMessageType messageType ) override;
    void Handle( const size_t index, MessageBase& message ) override;
};
