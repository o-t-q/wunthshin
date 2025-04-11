#include "../Public/dbcon.hpp"

std::unique_ptr<TableRegistrationStorage> G_TableTokenStorage         = {};

TableRegistrationStorage* AccessTableToken()
{
    if (!G_TableTokenStorage)
    {
        G_TableTokenStorage = std::make_unique<TableRegistrationStorage>();
    }

    return G_TableTokenStorage.get();
}

void Database::DBConnection::Clear( std::string_view table_name )
{
    if ( m_tables_.contains( table_name.data() ) )
    {
        GetTable( table_name )
                ->Execute<bool>(
                        [ &table_name ]( pqxx::work&& tx )
                        {
                            const pqxx::result result = tx.exec( std::format( "TRUNCATE TABLE {}", table_name ) );
                            tx.commit();
                            CONSOLE_OUT( __FUNCTION__, "Truncated table {}", table_name );
                            return true;
                        } );
    }
}
