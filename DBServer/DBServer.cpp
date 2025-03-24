#include <iostream>
#include <chrono>
#include "utility.hpp"
#include "boost-socket.hpp"
#include "message-handler.h"

int main()
{
    CONSOLE_OUT(__FUNCTION__, "Starting the Server");
    Network::NetworkContext TcpProtocol;
    MessageHandler          MessageHandler;

    TcpProtocol.accept( std::bind( &MessageHandler::Handle,
                                   &MessageHandler,
                                   std::placeholders::_1,
                                   std::placeholders::_2,
                                   std::placeholders::_3,
                                   std::placeholders::_4 ) );

}
