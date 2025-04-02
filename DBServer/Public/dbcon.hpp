#pragma once
#include <atomic>
#include <unordered_map>
#include <pqxx/connection>
#include <pqxx/pqxx>
#include "DBServer.h"
#include "utility.hpp"

namespace Database
{
    static bool DoesTableExists( pqxx::work&& tx, const std::string_view table_name )
    {
        return !tx.exec( "SELECT from pg_tables where tablename='$1';", pqxx::params{ table_name } ).empty();
    }

    struct Table;

    struct DBConnection
    {
        DBConnection() = delete;
        explicit DBConnection( const std::string_view db_name,
                               const uint16_t         port,
                               const std::string_view username,
                               const std::string_view password )
        {
            try
            {
                m_connection_ = std::make_unique<decltype( m_connection_ )::element_type>(
                        std::format( "dbname={} port={} user={} password={}", db_name, port, username, password ) );
                m_db_name_ = db_name;
                CONSOLE_OUT( __FUNCTION__, "Database connection has been established" );
                SanityCheck();
            }
            catch ( std::exception& e )
            {
                CONSOLE_OUT( __FUNCTION__, "Failed to connect to the database : {}", e.what() );
                std::abort();
            }
        }

        void SanityCheck() const
        {
            for ( const auto& name : m_tables_ | std::views::keys )
            {
                if ( !DoesTableExists( GetTransaction(), name ) )
                {
                    CONSOLE_OUT( __FUNCTION__, "Table sanity check failed : {}", name )
                    std::abort();
                }
            }
        }

        void RegisterTable( const std::string_view table_name )
        {
            if ( !m_tables_.contains( table_name.data() ) )
            {
                m_tables_.emplace( table_name.data(), make_vec_unique<Table>() );
            }
        }

        [[nodiscard]] Table* GetTable( const std::string_view table_name ) const
        {
            if ( !m_tables_.contains( table_name.data() ) )
            {
                return nullptr;
            }

            return &m_tables_.at( table_name.data() );
        }

    private:
        friend struct Table;

        [[nodiscard]] pqxx::work GetTransaction() const
        {
            return pqxx::work( *m_connection_ );
        }

        std::string                                             m_db_name_;
        std::unordered_map<std::string, accessor<Table>> m_tables_;
        std::unique_ptr<pqxx::connection>                       m_connection_;
    };

    struct Table final
    {
        virtual ~Table()       = default;
        
        Table()
        {
            m_lock_ = false;
        }

        Table(const Table& other) {}
        Table& operator=( const Table& other ) { return *this; }

        template <typename ReturnT, typename FuncT, typename... Args>
        [[nodiscard]] ReturnT Execute( FuncT func, Args&&... args ) const
        {
            bool success = false;
            bool lock    = false;
            while ( !m_lock_.compare_exchange_strong( lock, true ) )
            {
            }
            ReturnT result;

            try
            {
                auto tx = pqxx::work( GlobalScope::GetDatabase().GetTransaction() );
                result  = std::invoke( func, std::forward<Args>( args )...,  std::move( tx ) );
                success = true;
            }
            catch ( std::exception& e )
            {
                CONSOLE_OUT( __FUNCTION__, "Execute failed: {}", e.what() )
            }

            bool unlock = true;
            while ( !m_lock_.compare_exchange_strong( unlock, false ) )
            {
            }

            if ( success )
            {
                return result;
            }
            else
            {
                return {};
            }
        }

    private:
        mutable std::atomic<bool> m_lock_;
    };
}

template <typename TableT>
struct TableRegistration
{
    explicit TableRegistration(const std::string_view name)
    {
        GlobalScope::GetDatabase().RegisterTable( name );
    }
};