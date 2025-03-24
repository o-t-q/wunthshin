#pragma once
#define WIN32_MEAN_AND_LEAN
#include <boost/asio.hpp>

struct MessageHandler
{
    void Handle(
        size_t index, 
        const boost::asio::mutable_buffer& buffer, 
        const boost::system::error_code& ec, 
        size_t read );
};