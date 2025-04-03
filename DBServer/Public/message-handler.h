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

struct RegistrationToken
{
    virtual                                 ~RegistrationToken() = default;
    virtual accessor<HandlerImplementation> Initialize() const = 0;
};

struct HandlerRegistrationTokenStorage
{
    void RegisterHandler( RegistrationToken* handlerRegisterToken )
    {
        m_register_tokens_.emplace_back( handlerRegisterToken );
    }

    const std::vector<RegistrationToken*>& GetTokens()
    {
        return m_register_tokens_;
    }

private:
    std::vector<RegistrationToken*> m_register_tokens_;
};

extern std::unique_ptr<HandlerRegistrationTokenStorage> G_HandlerTokenStorage;

struct MessageHandler
{
    MessageHandler() = default;
    
    void Initialize();
    
    void Handle(
        size_t index, 
        const boost::asio::mutable_buffer& buffer, 
        const boost::system::error_code& ec, 
        size_t read );

private:
    std::vector<accessor<HandlerImplementation>> m_handlers_;
};

extern HandlerRegistrationTokenStorage* AccessHandlerToken();

template <typename T>
struct HandlerRegistration : RegistrationToken
{
    HandlerRegistration()
    {
        AccessHandlerToken()->RegisterHandler( this );
    }

    accessor<HandlerImplementation> Initialize() const override
    {
        return make_vec_unique<T>();
    }
};