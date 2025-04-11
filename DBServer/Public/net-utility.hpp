#pragma once
#include "utility.hpp"
#include "DBServer.h"
#include "boost-socket.hpp"

#undef SendMessage

template <typename T, typename... Args>
void SendMessage(const size_t to, Args&&... args)
{
    auto reply = make_vec_unique<T>( std::forward<Args>( args )... );
    GlobalScope::GetNetwork().send<T>( to, std::move( reply ) );
}
