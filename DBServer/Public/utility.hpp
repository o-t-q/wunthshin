#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <execution>

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

template <typename ContainerT>
    requires std::is_same_v<typename ContainerT::value_type, char>
bool check_null_trailing( const ContainerT& container, typename ContainerT::const_iterator& outIterator )
{
    outIterator = std::find( container.begin(), container.end(), '\0' );
    return outIterator != container.end();
}

template <typename ContainerT>
    requires std::is_same_v<typename ContainerT::value_type, char>
bool check_null_trailing( const ContainerT& container )
{
    const typename ContainerT::const_iterator& outIterator = std::find( container.begin(), container.end(), '\0' );
    return outIterator != container.end();
}

template <typename ContainerT>
inline bool is_null_container_unseq( const ContainerT& cont )
{
    return std::all_of( std::execution::par_unseq,
                        cont.begin(),
                        cont.end(),
                        []( const typename ContainerT::value_type& v )
                        { return v == ( typename ContainerT::value_type )0; } );
}
