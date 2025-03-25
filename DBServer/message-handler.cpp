#include "message-handler.h"
#include "message.h"

void MessageHandler::Handle( size_t                           index,
                             const boost::asio::mutable_buffer&     buffer,
                             const boost::system::error_code& ec,
                             const size_t                     read )
{
    if (read < 0)
    {
        // Invalid read
        return;
    }

    MessageBase* message = reinterpret_cast<MessageBase*>( buffer.data() );
    int32_t integerValue = static_cast<int32_t>( message->GetType() );
    
    if ( integerValue < 0 || integerValue >= GetMaxMessageIndex() )
    {
        // Unknown message
        return;
    }

    if ( G_MessageSize.at(integerValue) != read )
    {
        // invalid size
        return;
    }
}
