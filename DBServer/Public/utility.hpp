#pragma once
#include <mutex>
#include <stack>
#include <iostream>
#include <string>
#include <chrono>
#include <type_traits>
#include <execution>
#include <immintrin.h>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/pool.hpp>
#include <unordered_set>
#include <unordered_map>
#include <future>
#include <boost/container_hash/hash.hpp>
#include <boost/circular_buffer.hpp>

#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include <utility>

template <char... Cs>
struct TString
{
    constexpr static const char str[ sizeof...( Cs ) + 1 ] = { Cs..., '\0' };
        
    static constexpr std::string_view GetString() noexcept
    {
        return str;
    }
};

template <typename T, std::size_t... Is>
constexpr auto as_chars_impl( std::index_sequence<Is...> )
{
    return TString<T::str()[ Is ]...>{};
}

template <typename T>
constexpr auto as_chars()
{
    return as_chars_impl<T>( std::make_index_sequence<sizeof( T::str() ) - 1>{} );
}

#define STR( literal )                                                                                                 \
    []                                                                                                                 \
    {                                                                                                                  \
        struct literal_to_chars                                                                                        \
        {                                                                                                              \
            static constexpr decltype( auto ) str()                                                                    \
            {                                                                                                          \
                return literal;                                                                                        \
            }                                                                                                          \
        };                                                                                                             \
        return as_chars<literal_to_chars>();                                                                           \
    }()

struct LogFragmentBase
{
    virtual ~LogFragmentBase() = default;
    virtual void print()       = 0;
};

template <typename Format, typename... Args>
struct LogFragment : LogFragmentBase
{
    virtual ~LogFragment() override = default;

    constexpr LogFragment( std::string_view prefix, Args&&... args )
        : m_prefix_( prefix ), m_values_( std::forward<Args>( args )... )
    { }

    template <size_t... Index>
    void print_unwrap_tuple(std::index_sequence<Index...>)
    {
        std::cout << std::format( Format::GetString(), std::get<Index>( m_values_ )... ) << '\n';
    }

    void print() override
    {
        std::cout << std::format( "[{} | {}]: ",
                                  std::chrono::duration_cast<std::chrono::milliseconds>(
                                          std::chrono::high_resolution_clock::now().time_since_epoch() ),
                                  m_prefix_.data() );
        print_unwrap_tuple( std::make_index_sequence<std::tuple_size_v<decltype( m_values_ )>>{} );
    }

    std::string_view m_prefix_;
    std::tuple<Args...> m_values_;
};

struct Logger
{
    void start()
    {
        m_running_.store( true );
        m_consumer_thread_ = std::async( std::launch::async, &Logger::run, this );
    }

    void stop()
    {
        m_running_.store( false );
        m_consumer_thread_.wait();
    }

    template <typename Format, typename... Args>
    void push( std::string_view prefix, Args&&... args )
    {
        m_messages_.push_back(
                std::make_unique<LogFragment<Format, Args...>>( prefix, std::forward<Args>( args )... ) );
    }

private:
    void run()
    {
        while ( m_running_ )
        {
            if ( !m_messages_.empty() )
            {
                m_messages_.front()->print();
                m_messages_.pop_front();
            }
        }
    }

    std::atomic<bool>                                        m_running_;
    std::future<void>                                        m_consumer_thread_;
    boost::circular_buffer<std::unique_ptr<LogFragmentBase>> m_messages_{ 1 << 10 };
};

inline Logger S_Logger{};

#define CONSOLE_OUT(PREFIX, FORMAT, ...) \
    S_Logger.push<decltype( STR( FORMAT ) )>(##PREFIX, __VA_ARGS__ );


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
using vec_unique_ptr = std::unique_ptr<T, std::function<void( void* )>>;

struct __declspec( align( 32 ) ) alloc_pair
{
    size_t  chunk;
    uint8_t segment;
    uint8_t bit;

    bool operator==( const alloc_pair& other ) const
    {
        return chunk == other.chunk && segment == other.segment && bit == other.bit;
    }
};

template <typename T>
std::atomic<bool>& get_spinlock()
{
    static std::atomic<bool> lock;
    return lock;
}

template <typename T>
void do_lock( const bool value )
{
    std::atomic<bool>& mtx = get_spinlock<T>();

    bool expected = !value;
    while ( !mtx.compare_exchange_strong( expected, value ) )
    {
    }
}

#if defined(_DEBUG) && defined(WIN32)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>

struct debug_new_delete
{
    typedef std::size_t size_type;
    //!< An unsigned integral type that can represent the size of the largest object to be allocated.
    typedef std::ptrdiff_t difference_type;
    //!< A signed integral type that can represent the difference of any two pointers.

    static char* malloc( const size_type bytes )
    {
        //! Attempts to allocate n bytes from the system. Returns 0 if out-of-memory
#define DBG_NEW new ( _NORMAL_BLOCK, __FILE__, __LINE__ )
        // Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
        // allocations to be of _CLIENT_BLOCK type
        return DBG_NEW char[ bytes ];
#undef DBG_NEW
    }

    static void free( char* const block )
    {
        //! Attempts to de-allocate block.
        //! \pre Block must have been previously returned from a call to UserAllocator::malloc.
        delete [] block;
    }
};
using user_allocator = debug_new_delete;
#else
using user_allocator = boost::default_user_allocator_new_delete;
#endif

template <typename T,
    bool AllocateFast,
    typename UserAllocator = user_allocator,
    typename RebindFrom = T,
    typename Mutex = boost::details::pool::null_mutex, 
    unsigned NextSize = 32U, 
    unsigned MaxSize = 0U>
class tag_pool_alloc
{
    template <typename Type, bool AF>
    struct crowded_tag { };

public:
    inline static std::unordered_set<void(*)()> s_rebind_release = {};
    inline static std::unordered_set<void(*)()> s_rebind_purge = {};

    using pool_type = boost::singleton_pool<crowded_tag<RebindFrom, AllocateFast>, sizeof( T ), UserAllocator, Mutex, NextSize, MaxSize>;
    using value_type = T;
    using user_allocator = typename pool_type::user_allocator;
    using mutex = typename pool_type::mutex;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = typename pool_type::size_type;
    using difference_type = typename pool_type::difference_type;

    tag_pool_alloc()
    {
        pool_type::is_from( 0 );
    }

    template <typename U>
    tag_pool_alloc( const tag_pool_alloc<U, AllocateFast, UserAllocator, RebindFrom, Mutex, NextSize, MaxSize>& )
    {
        using origin_allocator = tag_pool_alloc<RebindFrom, AllocateFast, UserAllocator, RebindFrom, Mutex, NextSize, MaxSize>;
        origin_allocator::s_rebind_release.emplace( &release_memory );
        origin_allocator::s_rebind_purge.emplace( &purge_memory );
        pool_type::is_from( 0 );
    }

    template <typename U>
    struct rebind
    {
        typedef tag_pool_alloc<U, AllocateFast, UserAllocator, RebindFrom, Mutex, NextSize, MaxSize> other;
    };

    template <typename U, typename... Args>
    static void construct( U* ptr, Args&&... args ) 
    {
        if constexpr ( std::constructible_from<U, Args...> )
        {
            new (ptr) U( std::forward<Args>( args )... );                
        }
    }

    static void destroy( const pointer ptr )
    {
        if constexpr ( !std::is_same_v<T, RebindFrom> )
        {
            ptr->~T();   
        }
    }

    static pointer allocate( const size_type n )
    {
        if constexpr ( AllocateFast )
        {
            return ( pointer )pool_type::ordered_malloc( n );
        }
        else
        {
            if (n == 1)
            {
                return ( pointer )pool_type::malloc();
            }
            else
            {
                return ( pointer )pool_type::ordered_malloc( n );
            }
        }
    }
    static pointer allocate( const size_type n, const void* const )
    {
        return allocate( n );
    }

    static void deallocate(const pointer ptr)
    {
        if constexpr ( AllocateFast )
        {
            pool_type::ordered_free( ptr );   
        }
        else
        {
            pool_type::free( ptr );
        }
    }
    static void deallocate( const pointer ptr, const size_type n )
    {
        if constexpr ( AllocateFast )
        {
            pool_type::ordered_free( ptr, n );   
        }
        else
        {
            if (n == 1)
            {
                pool_type::free( ptr );
            }
            else
            {
                pool_type::free( ptr, n );
            }
        }
    }

    static void release_memory()
    {
        pool_type::release_memory();
    }

    static void purge_memory()
    {
        pool_type::purge_memory();
    }
};

struct pool_storage_base
{
    virtual ~pool_storage_base() = default;
    virtual void initialize()    = 0;
    virtual void destroy()       = 0;
};

struct storage_stack
{
    void add( pool_storage_base* pool_ptr )
    {
        do_lock<decltype( *this )>( true );
        m_loaded_pool_.emplace( pool_ptr );
        do_lock<decltype( *this )>( false );
    }

    bool contains(pool_storage_base* pool_ptr)
    {
        do_lock<decltype( *this )>( true );
        const bool result = m_loaded_pool_.contains( pool_ptr );
        do_lock<decltype( *this )>( false );
        return result;
    }

    void purge()
    {
        do_lock<decltype( *this )>( true );
        while ( !m_loaded_pool_.empty() )
        {
            pool_storage_base* pool = *m_loaded_pool_.begin();
            m_loaded_pool_.erase( m_loaded_pool_.begin() );
            pool->destroy();
        }
        do_lock<decltype( *this )>( false );
    }

private:
    template <typename StorageType, typename ValueAllocator>
    friend struct pool_storage_impl;

    void remove(pool_storage_base* pool_ptr)
    {
        do_lock<decltype( *this )>( true );
        m_loaded_pool_.erase( pool_ptr );
        do_lock<decltype( *this )>( false );
    }
    
    std::unordered_set<pool_storage_base*> m_loaded_pool_;
};

inline static storage_stack G_ManagedStorage = {};

template <typename StorageType, typename ValueAllocator>
struct pool_storage_impl : pool_storage_base
{
    using Allocator = tag_pool_alloc<StorageType, true>;

    ~pool_storage_impl() override
    {
        dispose();
    }

    auto& get()
    {
        if ( !pool || !m_initialized_ )
        {
            initialize();
        }

        return *pool;
    }

    void initialize() override
    {
        do_lock<decltype( *this )>( true );
        if ( !pool )
        {
            pool = Allocator::allocate( 1 );
            Allocator::construct( pool );
        }
        if ( !m_initialized_ )
        {
            pool->reserve( 1 << 10 );
            m_initialized_ = true;
        }
        do_lock<decltype( *this )>( false );

        G_ManagedStorage.add( this );
    }

    void destroy() override
    {
        do_lock<decltype( *this )>( true );

        if ( m_initialized_ )
        {
            if constexpr ( requires { pool->resize(0); } )
            {
                pool->resize( 0 );
                pool->shrink_to_fit();
            }
            else
            {
                pool->erase( pool->begin(), pool->end() );    
            }

            ValueAllocator::pool_type::release_memory();
            ValueAllocator::pool_type::purge_memory();
            m_initialized_ = false;

            if ( G_ManagedStorage.contains( this ) )
            {
                G_ManagedStorage.remove( this );
            }
        }

        do_lock<decltype( *this )>( false );
    }

    void dispose()
    {
        destroy();
        if ( pool )
        {
            Allocator::destroy( pool );
            Allocator::deallocate( pool );
            Allocator::release_memory();
            Allocator::purge_memory();
        }
        for (const auto& func : ValueAllocator::s_rebind_release)
        {
            func();
        }
        for (const auto& func : ValueAllocator::s_rebind_purge)
        {
            func();
        }
    }

private:
    bool m_initialized_ = false;
    StorageType* pool = nullptr;
};

template <typename T>
struct unordered_pool_storage : pool_storage_impl<std::unordered_set<T, std::hash<T>, std::equal_to<T>, tag_pool_alloc<T, false>>, tag_pool_alloc<T, false>>
{
};

template <typename KeyT, typename ValT, typename PairT = std::pair<const KeyT, ValT>>
struct unordered_map_pool_storage : pool_storage_impl<std::unordered_map<KeyT, ValT, std::hash<KeyT>, std::equal_to<KeyT>, tag_pool_alloc<PairT, false>>, tag_pool_alloc<PairT, false>>
{
};

template <typename T>
struct pool_storage : pool_storage_impl<std::vector<T, tag_pool_alloc<T, true>>, tag_pool_alloc<T, true>>
{
};

template <typename T>
auto& get_internal_storage()
{
    static pool_storage<T> storage;
    return storage.get();
}

template <typename KeyT, typename ValT>
auto& get_storage()
{
    static unordered_map_pool_storage<KeyT, ValT> storage;
    return storage.get();
}

template <typename T>
auto& get_instances()
{
    return get_storage<alloc_pair, vec_unique_ptr<T>>();
}

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
    auto& vec = get_internal_storage<T>();
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
                    new ( ptr ) T( std::forward<Args>( args )... );
                }
                
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

    T* get() const
    {
        if (T* ptr = &operator*())
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
