#pragma once
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <memory>
#include <vector>

#include "utility.hpp"
#include "DBServer.h"
#include "message.h"

struct HandlerImplementation
{
    virtual ~HandlerImplementation() = default;

    virtual bool ShouldHandle( EMessageType messageType )           = 0;
    virtual void Handle( const size_t index, MessageBase& message ) = 0;
};

struct MessageHandler
{
    MessageHandler() = default;
    void Handle(
        size_t index, 
        const boost::asio::mutable_buffer& buffer, 
        const boost::system::error_code& ec, 
        size_t read );

    template <typename T>
    void RegisterHandler()
    {
        m_handlers_.emplace_back( std::move( make_vec_unique<T>() ) );
    }

private:
    std::vector<accessor<HandlerImplementation>> m_handlers_;
};

template <typename T>
struct HandlerRegistration
{
    HandlerRegistration()
    {
        GlobalScope::GetHandler().RegisterHandler<T>();
    }
};
