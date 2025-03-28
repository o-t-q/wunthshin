#pragma once
#include <string>
#include <pqxx/pqxx>
#include "../../dbcon.hpp"

struct UserProfile
{
    std::string            name;
    std::string            email;
    std::array<std::byte, 32> hashedPassword;

    static bool   Insert( const UserProfile& table, pqxx::work&& tx )
    {
        pqxx::bytes_view   hashedPasswordView( table.hashedPassword.data(), table.hashedPassword.size() );
        const pqxx::result result = tx.exec( "INSERT INTO users VALUES (DEFAULT, $1, $2, $3);", { table.name, table.email, hashedPasswordView } );
        const size_t row_count = result.affected_rows();
        tx.commit();
        return row_count;
    }

    static bool FindName( const std::string_view name, pqxx::work&& tx )
    {
        const pqxx::result result = tx.exec( "SELECT FROM users WHERE name=$1", pqxx::params{ name } );
        return !result.empty();
    }

    static bool FindEmail( const std::string_view name, pqxx::work&& tx )
    {
        const pqxx::result result = tx.exec( "SELECT FROM users WHERE email=$1", pqxx::params{ name } );
        return !result.empty();
    }

    static size_t TryLoginStr( const std::string_view name, const std::array<std::byte, 32>& hashedPassword, pqxx::work&& tx )
    {
        const pqxx::params params{
            name, pqxx::bytes_view{ hashedPassword.begin(), hashedPassword.end() }
        };

        const auto result = tx.query_value<size_t>( "SELECT id FROM users WHERE name=$1 and password=$2;", params );
        return result;
    }

    static size_t TryLoginVarChar(const Varchar& name, const HashArray& hashedPassword, pqxx::work&& tx)
    {
        std::string stringify( name.begin(), name.end() );
        return TryLoginStr( stringify, hashedPassword, std::move( tx ) );
    }

    static size_t GetIdentifier( const std::string_view name, pqxx::work&& tx )
    {
        pqxx::params params;
        params.append( name );
        return tx.query_value<size_t>( "SELECT id FROM users WHERE name=$1;", params);
    }
};

static TableRegistration<UserProfile> UserTableRegistration( "User" );