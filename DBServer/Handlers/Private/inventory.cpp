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
    auto&      allItemMessage = CastTo<EMessageType::GetItemsRequest>( message );
    const auto userId         = login->GetLoginUser( allItemMessage.sessionId );

    const auto& replyFailed = [ &index, &allItemMessage ]()
    { SendMessage<GetItemsResponseMessage>( index, allItemMessage.sessionId, 0, false, true, 0, ItemArray{} );
    };

    if ( userId == -1 )
    {
        CONSOLE_OUT( __FUNCTION__, "Session ID {} does not found", to_hex_string( allItemMessage.sessionId ) )
        replyFailed();
        return;
    }

    const Database::Table* inventoryTable = GlobalScope::GetDatabase().GetTable( "inventory" );
    ItemArray              result{};
    size_t                 count = 0;
    bool                   isEnd          = false;

    if ( inventoryTable->Execute<bool>( &Inventory::GetAllItems, userId, allItemMessage.page, isEnd, count, result ) )
    {
        CONSOLE_OUT( __FUNCTION__, "Get Items request received, reply back to {}, count : {}", userId, count )
        SendMessage<GetItemsResponseMessage>(
                index, allItemMessage.sessionId, allItemMessage.page, true, isEnd, count, result );
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
    auto&      addItemMessage = CastTo<EMessageType::AddItem>( message );
    const auto userId         = login->GetLoginUser( addItemMessage.sessionId );

    const auto& replyFailed = [ &index, &addItemMessage ]()
    { SendMessage<AddItemResponseMessage>( index, addItemMessage.sessionId, false, EDBItemType::Unknown, -1, 0 ); };

    if ( userId == -1 )
    {
        CONSOLE_OUT( __FUNCTION__, "Session ID {} does not found", to_hex_string( addItemMessage.sessionId ) )
        replyFailed();
        return;
    }

    const Database::Table* inventoryTable = GlobalScope::GetDatabase().GetTable( "inventory" );

    if ( inventoryTable->Execute<bool>( &Inventory::NewItem, userId, addItemMessage.ItemType, addItemMessage.newItem, addItemMessage.count ) )
    {
        SendMessage<AddItemResponseMessage>( index,
                                             addItemMessage.sessionId,
                                             true,
                                             addItemMessage.ItemType,
                                             addItemMessage.newItem,
                                             addItemMessage.count );
        return;
    }
    else
    {
        replyFailed();
    }
}
