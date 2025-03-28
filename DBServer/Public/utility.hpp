#pragma once
#include <iostream>
#include <string>
#include <chrono>

#define CONSOLE_OUT(PREFIX, FMT, ...) \
    std::cout << std::format("[{} | {}]: ", std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ), PREFIX ); \
    std::cout << std::format(FMT, __VA_ARGS__) << '\n';

template <typename ContainerT> requires std::is_same_v<typename ContainerT::value_type, std::byte>
inline std::string to_hex_string(const ContainerT& byte_vec)
{
    std::stringstream ss;
    ss << std::hex;

    size_t idx = 0;
    for ( const std::byte& byte : byte_vec )
    {
        ss << std::setw(2) << std::setfill('0') << (int)byte;
    }

    return ss.str();
}