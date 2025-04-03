#pragma once

namespace Network
{
    template <uint16_t Port> struct NetworkContext;
}

struct MessageHandler;

namespace Database
{
    struct DBConnection;
}

struct GlobalScope
{
    static void                           Initialize();
    static void                           Destory();
    static Network::NetworkContext<1337>& GetNetwork();
    static MessageHandler&                GetHandler();
    static Database::DBConnection&        GetDatabase();

private:
    static std::unique_ptr<Network::NetworkContext<1337>> G_TcpProtocol;
    static std::unique_ptr<MessageHandler>                G_MessageHandler;
    static std::unique_ptr<Database::DBConnection>        G_Database;
};
