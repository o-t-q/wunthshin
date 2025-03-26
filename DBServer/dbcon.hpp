#pragma once
#include <pqxx/connection>
#include <pqxx/pqxx>
#include "utility.hpp"

namespace Database
{
	struct DBConnection
	{
        DBConnection() = delete;
        explicit DBConnection(const std::string_view db_name, const uint16_t port, const std::string_view username, const std::string_view password)
        {
            try
            {
                m_connection_ = std::make_unique<decltype( m_connection_ )::element_type>(
                        std::format( "dbname={} port={} user={} password={}", db_name, port, username, password ) );
                CONSOLE_OUT( __FUNCTION__, "Database connection has been established" );
            }
            catch (std::exception& e)
            {
                CONSOLE_OUT( __FUNCTION__, "Failed to connect to the database : {}", e.what() );
                std::abort();
            }
        }

        void SanityCheck()
        {
            pqxx::work tx( *m_connection_ );
        }

    private:
        std::unordered_map<std::string, std::vector<std::string>> m_tables_;
        std::unique_ptr<pqxx::connection> m_connection_;
	};
}