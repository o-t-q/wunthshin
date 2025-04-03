#pragma once
#include <mutex>
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
    std::cout << std::format("[{} | {}]: ", std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ), PREFIX ); \
    std::cout << std::format(FMT, __VA_ARGS__) << '\n';

struct message_tag { };

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

template <typename T, 
    typename UserAllocator = boost::default_user_allocator_new_delete, 
    typename Mutex = std::mutex, 
    unsigned NextSize = 32U, 
    unsigned MaxSize = 0U>
class pool_alloc_proxy
{
public:
    using pool_type = boost::pool_allocator<T, UserAllocator, Mutex, NextSize, MaxSize>;
    using value_type = typename pool_type::value_type;
    using user_allocator = typename pool_type::user_allocator;
    using mutex = typename pool_type::mutex;
    using pointer = typename pool_type::pointer;
    using const_pointer = typename pool_type::const_pointer;
    using reference = typename pool_type::reference;
    using const_reference = typename pool_type::const_reference;

    pool_alloc_proxy()
    {
        boost::singleton_pool<boost::pool_allocator_tag, sizeof( T ), UserAllocator, Mutex, NextSize, MaxSize>::is_from(
                0 );
    }

    template <typename U>
    pool_alloc_proxy( const pool_alloc_proxy<U, UserAllocator, Mutex, NextSize, MaxSize>& )
    {
        boost::singleton_pool<boost::pool_allocator_tag, sizeof( T ), UserAllocator, Mutex, NextSize, MaxSize>::is_from(
                0 );
    }

    template <typename U>
    struct rebind
    {
        typedef pool_alloc_proxy<U, UserAllocator, Mutex, NextSize, MaxSize> other;
    };

    template <typename U, typename... Args>
    static void construct( U* ptr, Args&&... args ) 
    { }

    static void destroy( const typename pool_type::pointer ptr ) { }

    static typename pool_type::pointer allocate( const typename pool_type::size_type n )
    {
        return pool_type::allocate( n );
    }
    static typename pool_type::pointer allocate( const typename pool_type::size_type n, const void* const )
    {
        return pool_type::allocate( n );
    }
    static void deallocate( const typename pool_type::pointer ptr, const typename pool_type::size_type n )
    {
        pool_type::deallocate( ptr, n );
    }
};

template <typename T>
auto& get_memory()
{
    static std::vector<T, pool_alloc_proxy<T>> pool;
    static std::once_flag                           init;
    std::call_once( init, []() { pool.reserve( 1 << 10 ); } );
    return pool;
}

template <typename T>
using vec_unique_ptr = std::unique_ptr<T, std::function<void(void*)>>;

struct __declspec( align( 32 ) ) alloc_pair
{
    size_t  chunk;
    uint8_t segment;
    uint8_t bit;

    bool operator==(const alloc_pair& other) const
    {
        return chunk == other.chunk && segment == other.segment && bit == other.bit;
    }
};

template <>
struct std::hash<alloc_pair>
{
    size_t operator()(const alloc_pair& key) const noexcept
    {
        std::hash<size_t> ull;
        std::hash<uint8_t> c;
        size_t             v = ull( key.chunk );
        boost::hash_combine( v, c( key.segment ) );
        boost::hash_combine( v, c( key.bit ) );
        return v;
    }
};

template <typename T>
auto& get_instances()
{
    static std::unordered_map<
        alloc_pair, 
        vec_unique_ptr<T>, 
        std::hash<alloc_pair>, 
        std::equal_to<alloc_pair>, 
        boost::fast_pool_allocator<std::pair<const alloc_pair, vec_unique_ptr<T>>>> instance_created;
    return instance_created;
}

template <typename T>
auto& get_alloc_masks()
{
    static std::vector<__m256i> alloc_mask{};
    return alloc_mask;
}

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

template <typename T>
void deallocate( T* ptr )
{
    auto& alloc_masks = get_alloc_masks<T>();
    auto& alloc_map   = get_alloc_map<T>();
    auto& instances   = get_instances<T>();
    do_lock<T>( true );
    assert( alloc_map.contains( ptr ) );
    const alloc_pair& mask = alloc_map.at( ptr );
    alloc_masks[ mask.chunk ].m256i_u32[ mask.segment ] &= ~( 1 << mask.bit );
    instances.erase( mask );
    alloc_map.erase( ptr );
    do_lock<T>( false );
    ptr->~T();
}

template <typename T, typename... Args>
T* allocate( alloc_pair& index, Args&&... args )
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
                index          = alloc_pair{ i, ( uint8_t )j, ( uint8_t )masks };
                T* ptr = nullptr;
                size_t prev_alloc_size = vec.capacity();

                if ( vec.size() <= vec_loc )
                {
                    ptr = &vec.emplace_back( std::forward<Args>( args )... );
                    if ( prev_alloc_size != vec.capacity() )
                    {
                        alloc_map.clear();
                        auto& instances = get_instances<T>();
                        for ( int k = 0; k < vec.size(); ++k )
                        {
                            alloc_pair idx{ k / sizeof( __m256i ),
                                              ( k / sizeof( uint32_t ) ) % ( sizeof( __m256i ) / sizeof(uint32_t) ),
                                              k % sizeof( uint32_t ) };
                            if ( !instances.contains(idx) )
                            {
                                continue;
                            }
                            new ( &instances.at( idx ) )
                                    vec_unique_ptr<T>( &vec[ k ], []( void* ptr ) { deallocate<T>( ( T* )ptr ); } );
                            alloc_map[ &vec[ k ] ] = idx;
                        }
                    }
                    alloc_map.emplace( ptr, index );
                }
                else
                {
                    ptr = &vec.at(vec_loc);
                }

                new ( ptr ) T( std::forward<Args>( args )... );
                alloc_map.emplace( ptr, index );

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
struct accessor
{
private:
    std::function<vec_unique_ptr<T>*( const alloc_pair& )> acc;
    alloc_pair                                             index{ ( size_t )-1, ( uint8_t )-1, ( uint8_t )-1 };

public:
    friend struct std::hash<accessor<T>>;

    struct cast_access
    {
        template <typename U>
            requires std::is_convertible_v<T*, U*>
        vec_unique_ptr<U>* get( const alloc_pair& index )
        {
            auto& instances = get_instances<T>();
            if ( instances.contains( index ) )
            {
                return ( vec_unique_ptr<U>* )&instances.at( index );
            }

            return nullptr;
        }
    };

    using accessor_type = cast_access;
    inline static accessor_type def_acc_for_t = {};

    accessor()
    {
        acc = []( const alloc_pair& index ) { return def_acc_for_t.get<T>( index ); };
        clear();
    }
    ~accessor()
    {
        reset();
    }

    void clear()
    {
        index = { ( size_t )-1, ( uint8_t )-1, ( uint8_t )-1 };
    }

    bool index_valid() const
    {
        if ( index.chunk == -1 || index.segment == -1 || index.bit == -1 )
        {
            return false;
        }

        return true;
    }

    bool allocation_valid() const
    {
        return acc( index ) != nullptr;
    }

    bool valid() const
    {
        return index_valid() && allocation_valid();
    }

    void reset()
    {
        if ( !valid() )
        {
            clear();
            return;
        }

        acc( index )->reset();
    }

    const alloc_pair& get_index() const
    {
        return index;
    }

    accessor& operator=( accessor&& other ) noexcept
    {
        acc   = other.acc;
        index = other.index;
        other.clear();
        return *this;
    }

    accessor(accessor&& other) noexcept
    {
        operator=( std::move( other ) );
    }

    template <typename U> requires std::is_convertible_v<U*, T*>
    accessor& operator=(accessor<U>&& other)
    {
        acc   = []( const alloc_pair& index ) { return accessor<U>::def_acc_for_t.get<T>( index ); };
        index = other.get_index();
        other.clear();
        return *this;
    }

    template <typename U> requires std::is_convertible_v<U*, T*>
    accessor(accessor<U>&& other)
    {
        operator=( std::move( other ) );
    }

    accessor( const accessor& )            = delete;
    accessor& operator=( const accessor& ) = delete;

    explicit accessor( const alloc_pair& index ) : index( index ) 
    {
        acc = []( const alloc_pair& index ) { return def_acc_for_t.get<T>( index ); };
    }

    bool operator==(const accessor& other) const noexcept
    {
        return index == other.index;
    }

    T* operator&()
    {
        if ( T* ptr = &operator*() )
        {
            return ptr;
        }

        return nullptr;
    }

    T* operator&() const
    {
        if ( T* ptr = &operator*() )
        {
            return ptr;
        }

        return nullptr;
    }

    T* operator->()
    {
        if ( T* ptr = &operator*() )
        {
            return ptr;
        }

        return nullptr;
    }

    T* operator->() const
    {
        if ( T* ptr = &operator*() )
        {
            return ptr;
        }

        return nullptr;
    }

    T& operator*()
    {
        if (index.chunk == -1 || index.segment == -1 || index.bit == -1)
        {
            assert( false );
        }

        return *acc( index )->get();
    }

    T& operator*() const
    {
        if ( index.chunk == -1 || index.segment == -1 || index.bit == -1 )
        {
            assert( false );
        }

        return *acc( index )->get();
    }
};

template <typename T>
struct std::hash<accessor<T>>
{
    size_t operator()(const accessor<T>& key) const noexcept
    {
        std::hash<alloc_pair> alloc_hash;
        return alloc_hash( key.index );
    }
};


template <typename T, typename Tag = void, typename... Args>
accessor<T> make_vec_unique( Args&&... args )
{
    alloc_pair        index;

    if constexpr (std::is_same_v<Tag, void>)
    {
        vec_unique_ptr<T> unique( allocate<T>( index, std::forward<Args>( args )... ),
                                  []( void* ptr ) { deallocate<T>( ( T* )ptr ); } );
        auto&             instances = get_instances<T>();
        instances.emplace( index, std::move( unique ) );
    }
    else
    {
        vec_unique_ptr<T> unique( allocate<Tag>( index, std::forward<Args>( args )... ),
                                  []( void* ptr ) { deallocate<T>( ( T* )ptr ); } );
        auto&             instances = get_instances<Tag>();
        instances.emplace( index, std::move( unique ) );
    }
    
    return accessor<T>{ index };
}