#pragma once

#include "../../Public/message.h"
#include "../../Public/message-handler.h"

struct RegisterHandler : HandlerImplementation
{
    bool ShouldHandle( EMessageType messageType ) override;
    void Handle( const size_t index, MessageBase& message ) override;
};
