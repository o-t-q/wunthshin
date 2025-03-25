#include <mutex>
#include <csignal>
#include <condition_variable>
#include "utility.hpp"
#include "boost-socket.hpp"
#include "message-handler.h"

Network::NetworkContext G_TcpProtocol;
MessageHandler          G_MessageHandler;

void CleanUp( int signal )
{
    CONSOLE_OUT( __FUNCTION__, "Signal received, cleanup..." );
    G_TcpProtocol.Destroy();
}

int main()
{
    CONSOLE_OUT(__FUNCTION__, "Starting the Server");
    std::mutex              SleepLock;
    std::unique_lock        Lock( SleepLock );
    std::condition_variable SleepCondVar;

    G_TcpProtocol.accept( std::bind( &MessageHandler::Handle,
                                   &G_MessageHandler,
                                   std::placeholders::_1,
                                   std::placeholders::_2,
                                   std::placeholders::_3,
                                   std::placeholders::_4 ) );

    std::signal( SIGINT | SIGTERM | SIGSEGV | SIGABRT | SIGFPE, CleanUp );

    SleepCondVar.wait( Lock );
}
