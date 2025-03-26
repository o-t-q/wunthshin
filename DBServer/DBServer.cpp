#include <mutex>
#include <csignal>
#include <condition_variable>

#include "message-handler.h"
#include "test_client.h"
#include "DBServer.h"
#include "utility.hpp"

#include "boost-socket.hpp"

std::unique_ptr<Network::NetworkContext<1337>> GlobalScope::G_TcpProtocol = {};

void CleanUp( int signal )
{
    CONSOLE_OUT( __FUNCTION__, "Signal received, cleanup..." );
    GlobalScope::GetNetwork().Destroy();
}

int main()
{
    CONSOLE_OUT(__FUNCTION__, "Starting the Server");
    std::mutex              SleepLock;
    std::unique_lock        Lock( SleepLock );
    std::condition_variable SleepCondVar;
    GlobalScope::Initialize();

    GlobalScope::GetNetwork().accept( std::bind( &MessageHandler::Handle,
                                                  &GlobalScope::GetHandler(),
                                                  std::placeholders::_1,
                                                  std::placeholders::_2,
                                                  std::placeholders::_3,
                                                  std::placeholders::_4 ) );

    std::signal( SIGINT | SIGTERM | SIGSEGV | SIGABRT | SIGFPE, CleanUp );

#ifdef _DEBUG
    StartTestClient();
#endif

    SleepCondVar.wait( Lock );
}

void GlobalScope::Initialize()
{
    GetNetwork();
    GetHandler();
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
