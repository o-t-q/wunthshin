#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <future>
#include <magic_enum/magic_enum.hpp>
#include "../Public/message.h"
#include "../Public/test_client.h"
#include "../Public/utility.hpp"

std::atomic<bool> ClientThreadRuninng;
std::future<void> FutureClientThread;

void RunClientThread()
{
    boost::asio::io_context context;
    boost::asio::ip::tcp::socket socket( context, { boost::asio::ip::tcp::v4(), 8889 } );
    boost::system::error_code    ec;
    socket.connect( { boost::asio::ip::make_address_v4( "127.0.0.1" ), 1337 }, ec );

    if (ec)
    {
        CONSOLE_OUT( __FUNCTION__, "Failed to connecting to the server : {}", ec.what() );
    }

    UUID sessionID;

    {
        RegisterMessage registerMessage;
        std::ranges::copy("name", registerMessage.name.begin());
        std::ranges::copy( "test@test.com", registerMessage.email.begin() );
        std::ranges::fill( registerMessage.hashedPassword, (std::byte)1 );
        boost::asio::const_buffer registerBuffer( &registerMessage, sizeof(registerMessage) );
        assert( socket.send( registerBuffer ) != 0 );
        CONSOLE_OUT( __FUNCTION__, "Register request sent" )
        RegisterStatusMessage registerResponse;
        boost::asio::mutable_buffer registerResponseBuffer( &registerResponse, sizeof( registerResponse ) );
        socket.receive( registerResponseBuffer );

        if (registerResponse.success)
        {
            CONSOLE_OUT( __FUNCTION__, "Register success" )
        }
        else
        {
            CONSOLE_OUT( __FUNCTION__, "Register failed, reason : {}", magic_enum::enum_name<ERegistrationFailCode>(registerResponse.code) )
        }
    }

    {
        LoginMessage loginMessage;
        constexpr char username[] = "name";
        strcpy_s( loginMessage.name.data(), std::size(username), username );
        std::ranges::fill(loginMessage.hashedPassword, (std::byte)1);
        boost::asio::const_buffer loginMessageBuffer( &loginMessage, sizeof( loginMessage ) );
        CONSOLE_OUT( __FUNCTION__, "Login request sent" );
        assert( socket.send( loginMessageBuffer ) != 0 );
        UUID container;
        LoginStatusMessage              loginReply{ std::move( container ) };
        boost::asio::mutable_buffer loginReceived( &loginReply, sizeof( loginReply ) );
        socket.receive( loginReceived );
        assert( loginReply.success );
        assert( !std::ranges::all_of( loginReply.sessionId.begin(), loginReply.sessionId.end(), [](const std::byte& b)
        {
            return b == (std::byte)0;
        } ) );
        CONSOLE_OUT( __FUNCTION__, "Login OK, Session ID : {}", to_hex_string( loginReply.sessionId ));
        sessionID = loginReply.sessionId;
    }

    {
        LogoutMessage logoutMessage( std::move( sessionID ) );
        boost::asio::const_buffer logoutBuffer ( &logoutMessage, sizeof( logoutMessage ) );
        assert( socket.send( logoutBuffer ) != 0 );
        CONSOLE_OUT( __FUNCTION__, "Logout request sent" );
        LogoutOKMessage logoutReply(false);
        boost::asio::mutable_buffer loginReceived( &logoutReply, sizeof( logoutReply ) );
        socket.receive( loginReceived );
        assert( logoutReply.success );
        CONSOLE_OUT( __FUNCTION__, "Logout OK" );
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
