#include <mutex>
#include <csignal>
#include <condition_variable>

#include "../Public/boost-socket.hpp"
#include "../Public/message-handler.h"
#include "../Public/dbcon.hpp"

#include "../Public/test_client.h"
#include "../Public/DBServer.h"
#include "../Public/utility.hpp"

#if defined(_DEBUG) && defined(WIN32)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>

void ReportMemoryLeak()
{
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtDumpMemoryLeaks();
    CONSOLE_OUT( __FUNCTION__, "Memory Leak Tested" );
}
#endif

void CleanUp( int signal )
{
    CONSOLE_OUT( __FUNCTION__, "Signal received, cleanup..." );
    GlobalScope::Destroy();

#ifdef _DEBUG
    ReportMemoryLeak();
#endif

    std::exit( signal );
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

    std::signal( SIGINT, CleanUp );
    std::signal( SIGTERM, CleanUp );
    std::signal( SIGABRT, CleanUp );
    std::signal( SIGSEGV, CleanUp );
    std::signal( SIGILL, CleanUp );
    std::signal( SIGABRT, CleanUp );
    std::signal( SIGFPE, CleanUp );

    SleepCondVar.wait( Lock );
    CleanUp( 0 );
}
