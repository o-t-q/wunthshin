#include <memory>
#include "../Public/message-handler.h"
#include "../Public/message.h"
#include "../Public/utility.hpp"

std::unique_ptr<MessageHandler> GlobalScope::G_MessageHandler = {};

void MessageHandler::Handle( size_t                           index,
                             const boost::asio::mutable_buffer&     buffer,
                             const boost::system::error_code& ec,
                             const size_t                     read )
{
    if (read <= 0)
    {
        // Invalid read
        CONSOLE_OUT( __FUNCTION__, "Invalid read size" )
        return;
    }

    MessageBase* message = reinterpret_cast<MessageBase*>( buffer.data() );
    int32_t integerValue = static_cast<int32_t>( message->GetType() );
    
    if ( integerValue < 0 || integerValue >= GetMaxMessageIndex() )
    {
        // Unknown message
        CONSOLE_OUT( __FUNCTION__, "Invalid message type" )
        return;
    }

    if ( G_MessageSize.at(integerValue) != read )
    {
        CONSOLE_OUT( __FUNCTION__, "Invalid packet size" )
        // invalid size
        return;
    }

    for (const std::unique_ptr<HandlerImplementation>& impl : m_handlers_)
    {
        if (impl->ShouldHandle(message->GetType()))
        {
            impl->Handle( index, *message );
        }
    }
}

void MessageHandler::RegisterHandler( HandlerImplementation* raw_ptr )
{
    if (std::find_if(m_handlers_.begin(), m_handlers_.end(), [ &raw_ptr ](const std::unique_ptr<HandlerImplementation>& element) 
        { return element.get() == raw_ptr;
        } ) == m_handlers_.end() )
    {
        m_handlers_.emplace_back( std::unique_ptr<HandlerImplementation>( raw_ptr ) );
    }
}
