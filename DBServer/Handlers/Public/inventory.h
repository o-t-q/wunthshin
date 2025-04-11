#pragma once
#include "../../Public/message-handler.h"

struct InventoryHandler : HandlerImplementation
{
    bool ShouldHandle( EMessageType messageType ) override;
    void Handle( const size_t index, MessageBase& message ) override;

private:
    void HandleAllItem( const size_t index, MessageBase& message );
    void HandleAddItem( const size_t index, MessageBase& message );
};
