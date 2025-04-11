#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <SDKDDKVer.h>

#include <boost/asio.hpp>
#include <boost/pool/object_pool.hpp>
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

        std::unordered_map<endpoint_type, size_t>                                   m_remote_endpoint_map_;
        std::unordered_map < size_t, accessor< typename Protocol::socket >> m_sockets_;
        std::unordered_map<size_t, boost::asio::mutable_buffer>                     m_recv_buffers_;
        std::unordered_map<size_t, std::unordered_set<accessor<MessageBase>>> m_pending_messages_;

        struct MessagePool
        {
#pragma pack( push, 1 )
            struct MessageBuffer
            {
                unsigned char data[ details::MaxPacketSize<Protocol>::value ];
            };
#pragma pack( pop )

            unsigned char* allocate()
            {
                return ( unsigned char* )message_buffer_pool.malloc();
            }

            void deallocate( unsigned char* buffer )
            {
                message_buffer_pool.free( ( MessageBuffer* )buffer );
            }

        private:
            boost::object_pool<MessageBuffer> message_buffer_pool;
        } m_message_buffer_pool_;


        size_t                                    m_next_idx_ = 0;
        accessor<typename Protocol::socket> m_pending_socket_;
        boost::asio::mutable_buffer               m_pending_buffer_;

        std::atomic<bool> m_accepting_;
        std::atomic<bool> m_running_;

    public:
        ~NetworkContext()
        {
            Destroy();
        }

        NetworkContext()
        {
            m_acceptor_ = std::make_unique<Protocol::acceptor>(
                    m_context_, Protocol::endpoint{ boost::asio::ip::tcp::v4(), Port } );

            m_local_endpoint_ = { boost::asio::ip::tcp::v4(), Port };

            const auto& thread_count = std::min<uint32_t>( std::thread::hardware_concurrency(), 4 );
            const auto& bind_error_handler =
                    std::bind( &NetworkContext::contextErrorHandler, std::placeholders::_1 );

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

            startAccept( std::move( predicate ) );

            m_running_.store( true );
        }

        template <typename MessageType>
            requires std::is_base_of_v<MessageBase, MessageType>
        bool send( const size_t index, accessor<MessageType>&& message )
        {
            if ( !m_running_ )
            {
                return false;
            }

#ifdef _DEBUG
            assert( m_sockets_.contains( index ) );
#endif
            if ( !m_sockets_.contains( index ) )
            {
                return false;
            }

            const auto& [ iter, _ ] = m_pending_messages_[ index ].emplace( std::move( message ) );
            boost::asio::const_buffer buffer( &( *iter ), sizeof( MessageType ) );

            // todo: fix to not use the pointer hack.
            m_sockets_.at( index )->async_send(
                    buffer,
                    [ this, index, iter ]( const boost::system::error_code& ec, const size_t sent )
                    {
                        m_pending_messages_.at( index ).erase( iter );
                    } );

            return true;
        }

        void Destroy()
        {
            m_running_ = false;
            m_context_.stop();

            std::ranges::for_each( m_io_threads_,
                                   []( std::thread& elem )
                                   {
                                       if ( elem.joinable() )
                                       {
                                           elem.join();
                                       }
                                   } );

            m_work_gurad_.reset();

            if (m_pending_buffer_.data())
            {
                m_message_buffer_pool_.deallocate( ( uint8_t* )m_pending_buffer_.data() );
            }
            m_pending_messages_.clear();
            m_pending_socket_.reset();

            for ( boost::asio::mutable_buffer& buffer : m_recv_buffers_ | std::views::values )
            {
                if ( buffer.data() )
                {
                    m_message_buffer_pool_.deallocate( ( uint8_t* )buffer.data() );
                }
            }

            m_recv_buffers_.clear();
        }

        uint16_t GetListenPort() const
        {
            return m_local_endpoint_.port();
        }

    private:
        void startAccept( const ReceiveHandlerSignature& predicate )
        {
            CONSOLE_OUT( __FUNCTION__, "Start Accepting" )

            m_pending_buffer_ = { m_message_buffer_pool_.allocate( ),
                                                 details::MaxPacketSize<Protocol>::value };
            m_pending_socket_ = make_vec_unique<Protocol::socket>( m_context_ );

            m_acceptor_->async_accept(
                    *m_pending_socket_,
                    [ this, predicate, index = m_next_idx_ ]( const boost::system::error_code& ec ) mutable
                    {
                        if ( !ec )
                        {
                            while ( m_sockets_.contains( index ) )
                            {
                                index++;
                            }

                            m_sockets_.emplace( index, std::move( m_pending_socket_ ) );
                            m_recv_buffers_.emplace( index, std::move( m_pending_buffer_ ) );
                            Protocol::endpoint endpoint = m_sockets_.at( index )->remote_endpoint();
                            CONSOLE_OUT( __FUNCTION__,
                                         "Accepting the connection of {}:{}",
                                         endpoint.address().to_v4().to_string(),
                                         endpoint.port() );

                            m_remote_endpoint_map_.emplace( endpoint, index );

                            m_sockets_.at( index )->async_receive( m_recv_buffers_[ index ],
                                                                   std::bind( &NetworkContext::receiveHandler,
                                                                              this,
                                                                              index,
                                                                              predicate,
                                                                              std::placeholders::_1,
                                                                              std::placeholders::_2 ) );

                            startAccept( predicate );
                        }
                    } );
            ++m_next_idx_;
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
                if ( !ec )
                {
#ifdef _DEBUG
                    assert( m_sockets_.contains( index ) );
#endif
                    m_sockets_.at( index )->async_read_some( m_recv_buffers_[ index ],
                                                             std::bind( &NetworkContext::receiveHandler,
                                                                        this,
                                                                        index,
                                                                        predicate,
                                                                        std::placeholders::_1,
                                                                        std::placeholders::_2 ) );
                }
                else
                {
                    if ( m_sockets_.at( index ).valid() )
                    {
                        if ( m_sockets_.at( index )->is_open() )
                        {
                            m_sockets_.at( index )->cancel();
                            m_sockets_.at( index )->close();
                            m_sockets_.at( index ).reset();
                        }
                    }

                    m_sockets_.erase( index );
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

        static void contextErrorHandler( const std::exception& e )
        {
            CONSOLE_OUT( __FUNCTION__, "contextRunner throws exception with {}", e.what() )
        }
    };
} // namespace Network
