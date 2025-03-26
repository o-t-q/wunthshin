#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <SDKDDKVer.h>

#include <boost/asio.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <array>
#include <thread>
#include <atomic>
#include <deque>
#include <vector>
#include <ranges>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "utility.hpp"
#include "message.h"

namespace Network
{
    namespace details
    {
#ifdef _WIN32
        struct winsock_udp_connreset
        {
            unsigned long value = 0;
            int           name()
            {
                return -1744830452; /* SIO_UDP_CONNRESET */
            }
            unsigned long* data()
            {
                return &value;
            }
        };
#endif

        using allocator_type = boost::fast_pool_allocator<unsigned char>;

        template <typename Protocol>
        static allocator_type& getAllocator()
        {
            static allocator_type alloc{};
            return alloc;
        }

        template <typename Protocol>
        static unsigned char* allocate( const size_t bytes )
        {
            return details::getAllocator<Protocol>().allocate( bytes );
        }

        template <typename Protocol>
        static void deallocate( unsigned char* ptr, const size_t bytes )
        {
            details::getAllocator<Protocol>().deallocate( ptr, bytes );
        }

        template <typename T>
        struct LocalAddressResolver
        {
            using endpoint_type = typename T::endpoint;

            std::array<uint8_t, 4> operator()()
            {
                const endpoint_type& endpoints = resolver.resolve( boost::asio::ip::host_name(), "" );

                for ( const endpoint_type& endpoint : endpoints )
                {
                    if ( endpoint.address().is_v4() && !endpoint.address().is_loopback() &&
                         !endpoint.address().is_unspecified() )
                    {
                        return endpoint.address().to_v4().to_bytes();
                    }
                }

                assert( nullptr );
                return {};
            }

        private:
            inline static boost::asio::io_context context{};
            inline static typename T::resolver    resolver{ context };
        };

        template <typename Protocol>
        struct MaxPacketSize
        { };

        template <>
        struct MaxPacketSize<boost::asio::ip::tcp>
        {
            static constexpr size_t value = std::numeric_limits<uint16_t>::max();
        };

        template <typename Protocol>
        struct Acceptor
        {
            void accept() { };
        };

        template <>
        struct Acceptor<boost::asio::ip::tcp>
        {
            using protocol_type = boost::asio::ip::tcp;
            using endpoint_type = protocol_type::endpoint;
            using socket_type   = protocol_type::socket;

            using ReadToken = std::function<void( const endpoint_type&, const boost::system::error_code& )>;

            Acceptor() : m_acceptor_( m_dummy_context_ ), m_initialized_( false )
            { }

            explicit Acceptor( boost::asio::io_context&              context,
                               boost::asio::ip::tcp::socket&         socket,
                               const boost::asio::ip::tcp::endpoint& endpoint )
                : m_acceptor_( context, endpoint ), m_socket_ptr_( &socket ), m_initialized_( true )
            { }

            void accept( ReadToken&& predicate )
            {
                // Not initialized
                assert( m_initialized_ );
                m_acceptor_.async_accept(
                        *m_socket_ptr_,
                        std::bind( predicate, m_socket_ptr_->remote_endpoint(), std::placeholders::_1 ) );
            }

            inline static boost::asio::io_context m_dummy_context_ = {};
            bool                                  m_initialized_;
            boost::asio::ip::tcp::acceptor        m_acceptor_;
            boost::asio::ip::tcp::socket*         m_socket_ptr_;
        };
    } // namespace details

    using ReceiveHandlerSignature =
            std::function<void( size_t, const boost::asio::mutable_buffer&, const boost::system::error_code&, size_t )>;

    template <uint16_t Port = 1337>
    struct NetworkContext
    {
    public:
        using Protocol      = boost::asio::ip::tcp;
        using endpoint_type = typename Protocol::endpoint;

    private:
        std::unique_ptr<Protocol::acceptor>                                     m_acceptor_;
        boost::asio::io_context                                                 m_context_;
        boost::asio::executor_work_guard<decltype( m_context_ )::executor_type> m_work_gurad_{
            boost::asio::make_work_guard( m_context_ )
        };
        std::vector<std::thread> m_io_threads_;
        endpoint_type            m_local_endpoint_;

        std::unordered_map<endpoint_type, size_t>                m_remote_endpoint_map_;
        std::unordered_map<size_t, typename Protocol::socket>    m_sockets_;
        std::unordered_map<size_t, std::atomic<bool>>            m_recv_running_;
        std::unordered_map<size_t, boost::asio::mutable_buffer>  m_recv_buffers_;
        std::unordered_map<size_t, std::unordered_set<std::unique_ptr<MessageBase>>> m_pending_messages_;

        std::atomic<bool> m_running_;

    public:
        ~NetworkContext()
        {
            Destroy();
        }

        NetworkContext()
        {
            m_acceptor_ = std::make_unique<Protocol::acceptor>(
                m_context_,
                Protocol::endpoint{ boost::asio::ip::tcp::v4(), Port }
            );

            m_local_endpoint_ = { boost::asio::ip::tcp::v4(), Port };

            const auto& thread_count = std::min<uint32_t>( std::thread::hardware_concurrency(), 4 );
            const auto& bind_error_handler =
                    std::bind( &NetworkContext::contextErrorHandler, this, std::placeholders::_1 );

            std::generate_n( std::back_inserter( m_io_threads_ ),
                             thread_count,
                             [ this, &bind_error_handler ]()
                             {
                                 return std::thread( &NetworkContext::contextRunner,
                                                     this,
                                                     std::ref( m_context_ ),
                                                     std::ref( bind_error_handler ) );
                             } );
        }

        void accept( const ReceiveHandlerSignature& predicate )
        {
            if ( m_running_ )
            {
                return;
            }

            const size_t receiving_threads = std::min<size_t>( std::thread::hardware_concurrency(), 4 );

            for ( size_t i = 0; i < receiving_threads; ++i )
            {
                startAccept( i, std::move( predicate ) );
            }

            m_running_.store( true );
        }

        template <typename MessageType> requires std::is_base_of_v <MessageBase, MessageType>
        bool send( const size_t index, std::unique_ptr<MessageType>&& message )
        {
            if ( !m_running_ )
            {
                return false;
            }

#ifdef _DEBUG
            assert( m_recv_running_.contains( index ) );
#endif
            if ( !m_recv_running_.at( index ) )
            {
                return false;
            }

            const auto& [iter, _] = m_pending_messages_[ index ].emplace( std::move( message ) );
            boost::asio::const_buffer buffer( ( *iter ).get(), sizeof( MessageType ) );
            const void*               unique_ptr_addr = reinterpret_cast<const void*>( &( *iter ) );

            m_sockets_.at( index ).async_send(
                    buffer,
                                            [ this, index, unique_ptr_addr ]( const boost::system::error_code& ec, const size_t sent ) 
                                            { 
                                                m_pending_messages_.at( index ).erase( *reinterpret_cast<const std::unique_ptr<MessageBase>*>( unique_ptr_addr ) );
                                            } );

            return true;
        }

        void Destroy()
        {
            m_running_ = false;

            for ( auto& [ index, flag ] : m_recv_running_ )
            {
                if ( flag )
                {
                    bool expected = true;
                    while ( !flag.compare_exchange_strong( expected, false ) )
                    {
                    }

                    m_sockets_.at( index ).cancel();
                    m_sockets_.at( index ).close();
                }
            }

            m_work_gurad_.reset();
            m_context_.stop();

            std::ranges::for_each( m_io_threads_,
                                   []( std::thread& elem )
                                   {
                                       if ( elem.joinable() )
                                       {
                                           elem.join();
                                       }
                                   } );

            for ( boost::asio::mutable_buffer& buffer : m_recv_buffers_ | std::views::values )
            {
                details::deallocate<Protocol>( ( uint8_t* )buffer.data(), buffer.size() );
            }
        }

        uint16_t GetListenPort() const
        {
            return m_local_endpoint_.port();
        }

    private:
        void startAccept( size_t i, const ReceiveHandlerSignature& predicate )
        {
            m_sockets_.emplace( i, Protocol::socket{ m_context_ } );
            if ( m_recv_buffers_[ i ].data() == nullptr )
            {
                m_recv_buffers_.at( i ) = boost::asio::mutable_buffer{
                                                 details::allocate<Protocol>( details::MaxPacketSize<Protocol>::value ),
                                                 details::MaxPacketSize<Protocol>::value };
            }

            CONSOLE_OUT( __FUNCTION__, "Start Accepting thread {}", i );
            m_acceptor_->async_accept( m_sockets_.at( i ),
                                      [ this, predicate, index = i ]( const boost::system::error_code& ec )
                                      {
                                          if ( !ec )
                                          {
                                              Protocol::endpoint endpoint = m_sockets_.at( index ).remote_endpoint();
                                              CONSOLE_OUT( __FUNCTION__,
                                                           "Accepting the connection of {}:{}",
                                                           endpoint.address().to_v4().to_string(),
                                                           endpoint.port() );
                                              m_remote_endpoint_map_.emplace( endpoint, index );
                                              if (m_recv_running_[index] == false)
                                              {
                                                  m_recv_running_.at( index ) = true;
                                              }
                                              m_sockets_.at( index ).async_receive(
                                                      m_recv_buffers_[ index ],
                                                      std::bind( &NetworkContext::receiveHandler,
                                                                 this,
                                                                 index,
                                                                 predicate,
                                                                 std::placeholders::_1,
                                                                 std::placeholders::_2 ) );
                                          }
                                      } );
        }

        void receiveHandler( const size_t                     index,
                             const ReceiveHandlerSignature&   predicate,
                             const boost::system::error_code& ec,
                             size_t                           read )
        {
#ifdef _DEBUG
            assert( m_recv_buffers_.contains( index ) );
#endif
            predicate( index, m_recv_buffers_.at( index ), ec, read );

            if constexpr ( std::is_same_v<Protocol, boost::asio::ip::tcp> )
            {
#ifdef _DEBUG
                assert( m_recv_running_.contains( index ) );
#endif
                if ( !ec && m_recv_running_.at( index ) )
                {
#ifdef _DEBUG
                    assert( m_sockets_.contains( index ) );
#endif
                    m_sockets_.at( index ).async_read_some( m_recv_buffers_[ index ],
                                                       std::bind( &NetworkContext::receiveHandler,
                                                                  this,
                                                                  index,
                                                                  predicate,
                                                                  std::placeholders::_1,
                                                                  std::placeholders::_2 ) );
                }
                else if ( ec )
                {
                    if ( m_running_ )
                    {
                        m_sockets_.at( index ).cancel();
                        m_sockets_.at( index ).close();
                        startAccept( index, predicate );
                    }
                }
            }
        }

        void contextRunner( boost::asio::io_context&                            context,
                            const std::function<void( const std::exception& )>& err_handler )
        {
            while ( true )
            {
                try
                {
                    context.run();
                    break;
                }
                catch ( std::exception& e )
                {
                    err_handler( e );
                }
            }
        }

        void contextErrorHandler( const std::exception& e )
        {
            CONSOLE_OUT( __FUNCTION__, "contextRunner throws exception with {}", e.what() )
        }
    };
} // namespace Network
