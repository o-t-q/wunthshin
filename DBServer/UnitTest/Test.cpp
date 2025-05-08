#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define BOOST_TEST_MODULE UnitTest
#include <boost/test/included/unit_test.hpp>
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <boost/mpl/range_c.hpp>
#include <csignal>
#include <future>

#include <magic_enum/magic_enum.hpp>
#include "../Public/DBServer.h"
#include "../Public/dbcon.hpp"
#include "../Public/boost-socket.hpp"
#include "../Public/message-handler.h"
#include "../Public/message.h"
#include "../Public/utility.hpp"

struct ServerClientFixture
{
    boost::asio::io_context      context;
    boost::asio::ip::tcp::socket socket{ context };

    ServerClientFixture()
    {
        CONSOLE_OUT( __FUNCTION__, "================ Test Start ================" )
        GlobalScope::Initialize();
        GlobalScope::GetNetwork().accept( std::bind( &MessageHandler::Handle,
                                                     &GlobalScope::GetHandler(),
                                                     std::placeholders::_1,
                                                     std::placeholders::_2,
                                                     std::placeholders::_3,
                                                     std::placeholders::_4 ) );

        socket = {
            context, { boost::asio::ip::tcp::v4(), 0 }
        };
        boost::system::error_code ec;
        socket.connect( { boost::asio::ip::make_address_v4( "127.0.0.1" ), 1337 }, ec );

        if ( ec )
        {
            CONSOLE_OUT( __FUNCTION__, "Failed to connecting to the server : {}", ec.what() );
        }
        BOOST_CHECK( !ec );
    }

    void CleanUp( int signal )
    {
        CONSOLE_OUT( __FUNCTION__, "Signal received, cleanup..." );
        GlobalScope::Destroy();
    }

    ~ServerClientFixture()
    {
        CleanUp( 0 );
    }
};

BOOST_FIXTURE_TEST_CASE( Register, ServerClientFixture )
{
    {
        RegisterMessage registerMessage;
        std::ranges::copy( "name", registerMessage.name.begin() );
        std::ranges::copy( "test@test.com", registerMessage.email.begin() );
        std::ranges::fill( registerMessage.hashedPassword, ( std::byte )1 );
        boost::asio::const_buffer registerBuffer( &registerMessage, sizeof( registerMessage ) );
        BOOST_CHECK( socket.send( registerBuffer ) != 0 );
        CONSOLE_OUT( __FUNCTION__, "Register request sent" )
        RegisterStatusMessage       registerResponse;
        boost::asio::mutable_buffer registerResponseBuffer( &registerResponse, sizeof( registerResponse ) );
        socket.receive( registerResponseBuffer );
        BOOST_CHECK( registerResponse.success );
        CONSOLE_OUT( __FUNCTION__, "Register success" )

        GlobalScope::GetDatabase().Clear( "users" );
        GlobalScope::GetDatabase().Clear( "inventory" );
    }
}

BOOST_FIXTURE_TEST_CASE( LoginAndOut, ServerClientFixture )
{
    {
        RegisterMessage registerMessage;
        std::ranges::copy( "name", registerMessage.name.begin() );
        std::ranges::copy( "test@test.com", registerMessage.email.begin() );
        std::ranges::fill( registerMessage.hashedPassword, ( std::byte )1 );
        boost::asio::const_buffer registerBuffer( &registerMessage, sizeof( registerMessage ) );
        BOOST_CHECK( socket.send( registerBuffer ) != 0 );
        CONSOLE_OUT( __FUNCTION__, "Register request sent" )
        RegisterStatusMessage       registerResponse;
        boost::asio::mutable_buffer registerResponseBuffer( &registerResponse, sizeof( registerResponse ) );
        socket.receive( registerResponseBuffer );
        BOOST_CHECK( registerResponse.success );
    }

    UUID sessionID;
    {
        LoginMessage   loginMessage;
        constexpr char username[] = "name";
        strcpy_s( loginMessage.name.data(), std::size( username ), username );
        std::ranges::fill( loginMessage.hashedPassword, ( std::byte )1 );
        boost::asio::const_buffer loginMessageBuffer( &loginMessage, sizeof( loginMessage ) );
        CONSOLE_OUT( __FUNCTION__, "Login request sent" );
        BOOST_CHECK( socket.send( loginMessageBuffer ) != 0 );
        LoginStatusMessage          loginReply{};
        boost::asio::mutable_buffer loginReceived( &loginReply, sizeof( loginReply ) );
        socket.receive( loginReceived );
        BOOST_CHECK( loginReply.success );
        BOOST_CHECK( !std::ranges::all_of( loginReply.sessionId.begin(),
                                           loginReply.sessionId.end(),
                                           []( const std::byte& b ) { return b == ( std::byte )0; } ) );
        CONSOLE_OUT( __FUNCTION__, "Login OK, Session ID : {}", to_hex_string( loginReply.sessionId ) );
        sessionID = loginReply.sessionId;
    }

    {
        LogoutMessage             logoutMessage( sessionID );
        boost::asio::const_buffer logoutBuffer( &logoutMessage, sizeof( logoutMessage ) );
        BOOST_CHECK( socket.send( logoutBuffer ) != 0 );
        CONSOLE_OUT( __FUNCTION__, "Logout request sent" );
        LogoutOKMessage             logoutReply( false, UUID() );
        boost::asio::mutable_buffer loginReceived( &logoutReply, sizeof( logoutReply ) );
        socket.receive( loginReceived );
        BOOST_CHECK( logoutReply.success );
        CONSOLE_OUT( __FUNCTION__, "Logout OK" );
    }

    GlobalScope::GetDatabase().Clear( "users" );
    GlobalScope::GetDatabase().Clear( "inventory" );
}

BOOST_FIXTURE_TEST_CASE( PingPong, ServerClientFixture )
{
    PingPongMessage           Message;
    boost::asio::const_buffer request( &Message, sizeof( Message ) );
    CONSOLE_OUT( __FUNCTION__, "Client send the ping" );
    BOOST_CHECK( socket.send( request ) != 0 );
    PingPongMessage             Reply;
    boost::asio::mutable_buffer received( &Reply, sizeof( Reply ) );
    socket.receive( received );
    CONSOLE_OUT( __FUNCTION__, "Client received the pong" );
}

BOOST_FIXTURE_TEST_CASE( AddItemToInventory, ServerClientFixture )
{
    {
        RegisterMessage registerMessage;
        std::ranges::copy( "name", registerMessage.name.begin() );
        std::ranges::copy( "test@test.com", registerMessage.email.begin() );
        std::ranges::fill( registerMessage.hashedPassword, ( std::byte )1 );
        boost::asio::const_buffer registerBuffer( &registerMessage, sizeof( registerMessage ) );
        BOOST_CHECK( socket.send( registerBuffer ) != 0 );
        CONSOLE_OUT( __FUNCTION__, "Register request sent" )
        RegisterStatusMessage       registerResponse;
        boost::asio::mutable_buffer registerResponseBuffer( &registerResponse, sizeof( registerResponse ) );
        socket.receive( registerResponseBuffer );
        BOOST_CHECK( registerResponse.success );
    }
    UUID sessionID;
    {
        LoginMessage   loginMessage;
        constexpr char username[] = "name";
        strcpy_s( loginMessage.name.data(), std::size( username ), username );
        std::ranges::fill( loginMessage.hashedPassword, ( std::byte )1 );
        boost::asio::const_buffer loginMessageBuffer( &loginMessage, sizeof( loginMessage ) );
        CONSOLE_OUT( __FUNCTION__, "Login request sent" );
        BOOST_CHECK( socket.send( loginMessageBuffer ) != 0 );
        LoginStatusMessage          loginReply{};
        boost::asio::mutable_buffer loginReceived( &loginReply, sizeof( loginReply ) );
        socket.receive( loginReceived );
        BOOST_CHECK( loginReply.success );
        BOOST_CHECK( !std::ranges::all_of( loginReply.sessionId.begin(),
                                           loginReply.sessionId.end(),
                                           []( const std::byte& b ) { return b == ( std::byte )0; } ) );
        CONSOLE_OUT( __FUNCTION__, "Login OK, Session ID : {}", to_hex_string( loginReply.sessionId ) );
        sessionID = loginReply.sessionId;
    }
    {
        AddItemRequestMessage Message;
        Message.newItem   = 1;
        Message.count     = 1;
        Message.sessionId = sessionID;
        boost::asio::const_buffer request( &Message, sizeof( Message ) );
        CONSOLE_OUT( __FUNCTION__, "Client send the add item" );
        BOOST_CHECK( socket.send( request ) != 0 );
        AddItemResponseMessage      Reply;
        boost::asio::mutable_buffer received( &Reply, sizeof( Reply ) );
        socket.receive( received );
        CONSOLE_OUT( __FUNCTION__, "Client received the add item result" );
        BOOST_CHECK( Reply.itemID == Message.newItem );
    }

    GlobalScope::GetDatabase().Clear( "users" );
    GlobalScope::GetDatabase().Clear( "inventory" );
}

struct MessageTestPredicator
{
    template <int Value>
    void operator()( boost::mpl::integral_c<int, Value> iterator) const
    {
        using actual_type = MessageTypeResolver<( EMessageType )Value>::type;
        using func_invoke_result = std::invoke_result_t<decltype(CastTo<( EMessageType )Value>), MessageBase&>;
        constexpr static bool test = std::is_same_v<actual_type, std::remove_reference_t<func_invoke_result>>;

        CONSOLE_OUT( __FUNCTION__,
                     "{} : {}, match : {}",
                     typeid( actual_type ).name(),
                     typeid( func_invoke_result ).name(),
                     test )
        BOOST_CHECK( test );
    }
};

BOOST_AUTO_TEST_CASE( MessageCastTest )
{
    CONSOLE_OUT( __FUNCTION__, "================ Test Start ================" )
    boost::mpl::for_each<boost::mpl::range_c<int, 0, (int)EMessageType::MAX>>(
            MessageTestPredicator() );
}

BOOST_FIXTURE_TEST_CASE( GetItemsFromInventory, ServerClientFixture )
{
    {
        RegisterMessage registerMessage;
        std::ranges::copy( "name", registerMessage.name.begin() );
        std::ranges::copy( "test@test.com", registerMessage.email.begin() );
        std::ranges::fill( registerMessage.hashedPassword, ( std::byte )1 );
        boost::asio::const_buffer registerBuffer( &registerMessage, sizeof( registerMessage ) );
        BOOST_CHECK( socket.send( registerBuffer ) != 0 );
        CONSOLE_OUT( __FUNCTION__, "Register request sent" )
        RegisterStatusMessage       registerResponse;
        boost::asio::mutable_buffer registerResponseBuffer( &registerResponse, sizeof( registerResponse ) );
        socket.receive( registerResponseBuffer );
        BOOST_CHECK( registerResponse.success );
    }
    UUID sessionID;
    {
        LoginMessage   loginMessage;
        constexpr char username[] = "name";
        strcpy_s( loginMessage.name.data(), std::size( username ), username );
        std::ranges::fill( loginMessage.hashedPassword, ( std::byte )1 );
        boost::asio::const_buffer loginMessageBuffer( &loginMessage, sizeof( loginMessage ) );
        CONSOLE_OUT( __FUNCTION__, "Login request sent" );
        BOOST_CHECK( socket.send( loginMessageBuffer ) != 0 );
        LoginStatusMessage          loginReply{};
        boost::asio::mutable_buffer loginReceived( &loginReply, sizeof( loginReply ) );
        socket.receive( loginReceived );
        BOOST_CHECK( loginReply.success );
        BOOST_CHECK( !std::ranges::all_of( loginReply.sessionId.begin(),
                                           loginReply.sessionId.end(),
                                           []( const std::byte& b ) { return b == ( std::byte )0; } ) );
        CONSOLE_OUT( __FUNCTION__, "Login OK, Session ID : {}", to_hex_string( loginReply.sessionId ) );
        sessionID = loginReply.sessionId;
    }
    {
        AddItemRequestMessage Message;
        Message.ItemType  = EDBItemType::Consumable;
        Message.newItem   = 1;
        Message.count     = 1;
        Message.sessionId = sessionID;
        boost::asio::const_buffer request( &Message, sizeof( Message ) );
        CONSOLE_OUT( __FUNCTION__, "Client send the add item" );
        BOOST_CHECK( socket.send( request ) != 0 );
        AddItemResponseMessage      Reply;
        boost::asio::mutable_buffer received( &Reply, sizeof( Reply ) );
        socket.receive( received );
        CONSOLE_OUT( __FUNCTION__, "Client received the add item result" );
        BOOST_CHECK( Reply.Success );
    }

    {
        GetItemsRequestMessage Message;
        Message.page      = 0;
        Message.sessionId = sessionID;
        boost::asio::const_buffer request( &Message, sizeof( Message ) );
        CONSOLE_OUT( __FUNCTION__, "Client send the get item" );
        BOOST_CHECK( socket.send( request ) != 0 );
        GetItemsResponseMessage     Reply;
        boost::asio::mutable_buffer received( &Reply, sizeof( Reply ) );
        socket.receive( received );
        CONSOLE_OUT( __FUNCTION__, "Client received the get item result" );
        BOOST_CHECK( Reply.success );
        ItemAndCount Empty{};

        CONSOLE_OUT( __FUNCTION__,
                     "Inventory Result : Type: {}, ID: {}, Count: {}",
                     magic_enum::enum_name<EDBItemType>(Reply.items.begin()->ItemType),
                     Reply.items.begin()->ItemID,
                     Reply.items.begin()->Count )

        bool test = std::all_of( Reply.items.begin(),
                                 Reply.items.end(),
                                 [ &Empty ]( const ItemAndCount& Element )
                                 { return Element.ItemType == EDBItemType::Unknown && Element.ItemID == Empty.ItemID && Element.Count == Empty.Count; } );
        BOOST_CHECK( !test );
    }
    
    GlobalScope::GetDatabase().Clear( "users" );
    GlobalScope::GetDatabase().Clear( "inventory" );
}
