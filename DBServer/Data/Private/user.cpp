#include "../Public/user.hpp"

TableRegistration<UserProfile> UserTableRegistration("User");

bool UserProfile::Insert( const UserProfile& table, pqxx::work&& tx )
{
    pqxx::bytes_view   hashedPasswordView( table.hashedPassword.data(), table.hashedPassword.size() );
    const pqxx::result result = tx.exec( "INSERT INTO users VALUES (DEFAULT, $1, $2, $3);", { table.name, table.email, hashedPasswordView } );
    const size_t row_count = result.affected_rows();
    tx.commit();
    return row_count;
}

size_t UserProfile::TryLogin( const std::string_view        name,
                              const std::array<std::byte, 32>& hashedPassword,
                              pqxx::work&&                  tx )
{
    const pqxx::params params { name, pqxx::bytes_view{ hashedPassword.begin(), hashedPassword.end() } };

    const auto             result = tx.query_value<size_t>( "SELECT id FROM users WHERE name=$1 and password=$2;", params );
    return result;
}

size_t UserProfile::GetIdentifier( const std::string_view name, pqxx::work&& tx )
{
    pqxx::params params;
    params.append( name );
    return tx.query_value<size_t>( "SELECT id FROM users WHERE name=$1;", params);
}
