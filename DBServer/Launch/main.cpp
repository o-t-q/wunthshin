#include <mutex>
#include <csignal>
#include <condition_variable>

#include "../Public/boost-socket.hpp"
#include "../Public/message-handler.h"
#include "../Public/dbcon.hpp"

#include "../Public/test_client.h"
#include "../Public/DBServer.h"
#include "../Public/utility.hpp"

void CleanUp( int signal )
{
    CONSOLE_OUT( __FUNCTION__, "Signal received, cleanup..." );
    GlobalScope::Destroy();
}

int main()
{
    std::ios::sync_with_stdio( false );
    CONSOLE_OUT( __FUNCTION__, "Starting the Server" );
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

    SleepCondVar.wait( Lock );
}
