#include <mutex>
#include <csignal>
#include <condition_variable>

#include "../Public/boost-socket.hpp"
#include "../Public/message-handler.h"
#include "../Public/dbcon.hpp"

#include "../Public/test_client.h"
#include "../Public/DBServer.h"
#include "../Public/utility.hpp"

std::unique_ptr<Network::NetworkContext<1337>> GlobalScope::G_TcpProtocol = {};
std::unique_ptr<Database::DBConnection>        GlobalScope::G_Database    = {};

void GlobalScope::Initialize()
{
    GetNetwork();
    GetHandler().Initialize();
    GetDatabase().Initialize();
}

void GlobalScope::Destroy()
{
    G_TcpProtocol.reset();
    G_Database.reset();
    G_MessageHandler.reset();
}

Network::NetworkContext<1337>& GlobalScope::GetNetwork()
{
    if (!G_TcpProtocol)
    {
        G_TcpProtocol = std::make_unique<Network::NetworkContext<1337>>();
    }

    return *G_TcpProtocol;
}

MessageHandler& GlobalScope::GetHandler()
{
    if (!G_MessageHandler)
    {
        G_MessageHandler = std::make_unique<MessageHandler>();
    }

    return *G_MessageHandler;
}

Database::DBConnection& GlobalScope::GetDatabase()
{
    if (!G_Database)
    {
        G_Database = std::make_unique<Database::DBConnection>( "postgres", 5432, "postgres", "1234" );
    }

    return *G_Database;
}
