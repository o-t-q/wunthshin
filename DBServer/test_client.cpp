#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <future>
#include "message.h"
#include "test_client.h"
#include "utility.hpp"

std::atomic<bool> ClientThreadRuninng;
std::future<void> FutureClientThread;

void RunClientThread()
{
    boost::asio::io_context context;
    boost::asio::ip::tcp::socket socket( context, { boost::asio::ip::tcp::v4(), 8888 } );
    boost::system::error_code    ec;
    socket.connect( { boost::asio::ip::make_address_v4( "127.0.0.1" ), 1337 }, ec );

    if (ec)
    {
        CONSOLE_OUT( __FUNCTION__, "Failed to connecting to the server : {}", ec.what() );
    }

    while ( ClientThreadRuninng )
    {
        PingPongMessage Message;
        boost::asio::const_buffer request( &Message, sizeof( Message ) );
        CONSOLE_OUT( __FUNCTION__, "Client send the ping" );
        assert( socket.send( request ) != 0 );
        PingPongMessage             Reply;
        boost::asio::mutable_buffer received( &Reply, sizeof( Reply ) );
        socket.receive( received );
        CONSOLE_OUT( __FUNCTION__, "Client received the pong" );
        std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
    }
}

void StartTestClient()
{
    ClientThreadRuninng = true;
    FutureClientThread = std::async( std::launch::async, RunClientThread );
}

void StopTestClient()
{
    if ( FutureClientThread.valid() )
    {
        bool Expected = true;
        while ( !ClientThreadRuninng.compare_exchange_strong( Expected, false ) )
        { }
        FutureClientThread.wait();
    }
}
