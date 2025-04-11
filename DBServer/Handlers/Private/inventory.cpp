#include "../Public/inventory.h"
#include "../Public/login.h"
#include "../../Public/boost-socket.hpp"
#include "../../Public/dbcon.hpp"
#include "../../Data/Public/inventory.hpp"
#include "../../Public/net-utility.hpp"

HandlerRegistration<InventoryHandler> InventoryHandlerRegistration( "inventory" );

bool InventoryHandler::ShouldHandle( EMessageType messageType )
{
    return messageType == EMessageType::GetItemsRequest || messageType == EMessageType::AddItem;
}

void InventoryHandler::Handle( const size_t index, MessageBase& message )
{
    switch (message.GetType())
    {
    case EMessageType::GetItemsRequest:
            HandleAllItem( index, message );
            break;
    case EMessageType::AddItem:
            HandleAddItem( index, message );
            break;
    }
}

void InventoryHandler::HandleAllItem( const size_t index, MessageBase& message )
{ 
    const auto login          = GlobalScope::GetHandler().GetHandler<LoginHandler>( "login" );
    auto&      allItemMessage = reinterpret_cast<GetItemsRequestMessage&>( message );
    const auto userId         = login->GetLoginUser( allItemMessage.sessionId );

    const auto& replyFailed = [ &index ]()
    {
        SendMessage<GetItemsResponseMessage>( index, 0, false, true, ItemArray{} );
    };

    if ( userId == -1 )
    {
        CONSOLE_OUT( __FUNCTION__, "Session ID {} does not found", to_hex_string( allItemMessage.sessionId ) )
        replyFailed();
        return;
    }

    const Database::Table* inventoryTable = GlobalScope::GetDatabase().GetTable( "inventory" );
    ItemArray              result{};
    bool                   isEnd          = false;

    if ( inventoryTable->Execute<bool>( &Inventory::GetAllItems, userId, allItemMessage.page, isEnd, result ) )
    {
        SendMessage<GetItemsResponseMessage>( index, allItemMessage.page, true, isEnd, result );
    }
    else
    {
        CONSOLE_OUT( __FUNCTION__, "User {} requested empty page {}", userId, allItemMessage.page )
        replyFailed();
    }
}

void InventoryHandler::HandleAddItem( const size_t index, MessageBase& message )
{
    const auto login          = GlobalScope::GetHandler().GetHandler<LoginHandler>( "login" );
    auto&      addItemMessage = reinterpret_cast<AddItemRequestMessage&>( message );
    const auto userId         = login->GetLoginUser( addItemMessage.sessionId );

    const auto& replyFailed = [ &index ]() { SendMessage<AddItemResponseMessage>( index, false ); };

    if ( userId == -1 )
    {
        CONSOLE_OUT( __FUNCTION__, "Session ID {} does not found", to_hex_string( addItemMessage.sessionId ) )
        replyFailed();
        return;
    }

    const Database::Table* inventoryTable = GlobalScope::GetDatabase().GetTable( "inventory" );

    if ( inventoryTable->Execute<bool>( &Inventory::NewItem, userId, addItemMessage.newItem, addItemMessage.count ) )
    {
        SendMessage<AddItemResponseMessage>( index, true );
        return;
    }
    else
    {
        replyFailed();
    }
}
