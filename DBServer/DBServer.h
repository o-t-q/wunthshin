#pragma once

namespace Network
{
    template <uint16_t Port> struct NetworkContext;
}

struct MessageHandler;

struct GlobalScope
{
    static void                           Initialize();
    static Network::NetworkContext<1337>& GetNetwork();
    static MessageHandler&                GetHandler();

private:
    static std::unique_ptr<Network::NetworkContext<1337>> G_TcpProtocol;
    static std::unique_ptr<MessageHandler>                G_MessageHandler;
};
