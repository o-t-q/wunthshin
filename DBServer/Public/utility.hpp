#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <execution>
#include <immintrin.h>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/pool.hpp>
#include <unordered_set>
#include <unordered_map>
#include <boost/container_hash/hash.hpp>

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

static const __m256i mm256_all_mask = _mm256_set1_epi32( std::numeric_limits<uint32_t>::max() );

template <typename T>
std::atomic<bool>& get_spinlock()
{
    static std::atomic<bool> lock;
    return lock;
}

template <typename T>
auto& get_memory()
{
    static std::vector<T, boost::pool_allocator<T>> pool;
    return pool;
}

template <typename T>
auto& get_alloc_masks()
{
    static std::vector<__m256i> alloc_mask{};
    return alloc_mask;
}

struct __declspec( align( 32 ) ) alloc_pair
{
    size_t chunk;
    uint8_t segment;
    uint8_t bit;
};

template <typename T>
auto& get_alloc_map()
{
    static std::unordered_map<T*, alloc_pair> alloc_map;
    return alloc_map;
}

template <typename T>
void do_lock( const bool value )
{
    std::atomic<bool>& mtx = get_spinlock<T>();

    bool expected = !value;
    while ( !mtx.compare_exchange_strong( expected, value ) ) {}
}

template <typename T, typename... Args>
T* allocate( Args&&... args )
{
    auto& vec = get_memory<T>();
    auto& alloc_masks = get_alloc_masks<T>();
    auto& alloc_map = get_alloc_map<T>();

    do_lock<T>( true );

    if ( alloc_masks.empty() || _mm256_testc_si256( alloc_masks.back(), mm256_all_mask ) )
    {
        alloc_masks.emplace_back( __m256i{} );
    }
    
    for ( size_t i = 0; i < alloc_masks.size(); ++i )
    {
        if ( !_mm256_testc_si256( alloc_masks[i], mm256_all_mask ) )
        {
            for ( int j = 0; j < std::numeric_limits<uint32_t>::digits; ++j )
            {
                if ( alloc_masks[i].m256i_u32[j] == std::numeric_limits<uint32_t>::max() )
                {
                    continue;
                }

                const uint32_t masks = _tzcnt_u32( ~alloc_masks[i].m256i_u32[j] );
                alloc_masks[i].m256i_u32[j] |= 1 << masks;
                size_t vec_loc = ( i * sizeof(__m256i) ) + ( sizeof(uint32_t) * j ) + masks;
                T* ptr = nullptr;
                if ( vec.size() <= vec_loc )
                {
                    ptr = &vec.emplace_back( std::forward<Args>( args )... );
                }
                else
                {
                    ptr = &vec.at(vec_loc);
                    new(ptr)T( std::forward<Args>( args )... );
                }
                alloc_map.emplace( ptr, alloc_pair{ i, (uint8_t)j, (uint8_t)masks } );
                do_lock<T>( false );
                return ptr;
            }
        }
    }

    do_lock<T>( false );
    assert( false );
    return nullptr;
}

template <typename T>
void deallocate(T* ptr)
{
    auto& alloc_masks = get_alloc_masks<T>();
    auto& alloc_map = get_alloc_map<T>();
    do_lock<T>( true );
    assert( alloc_map.contains( ptr ) );
    const alloc_pair& mask = alloc_map.at( ptr );
    alloc_masks[ mask.chunk ].m256i_u32[ mask.segment ] &= ~( 1 << mask.bit );
    alloc_map.erase( ptr );
    do_lock<T>( false );
    ptr->~T();
}

template <typename T>
using vec_unique_ptr = std::unique_ptr<T, std::function<void(void*)>>;

template <typename T, typename... Args>
vec_unique_ptr<T> make_vec_unqiue( Args&&... args )
{
    vec_unique_ptr<T> unique( allocate<T>( std::forward<Args>( args )... ), [](void* ptr) { deallocate<T>( (T*)ptr ); } );
    return unique;
}