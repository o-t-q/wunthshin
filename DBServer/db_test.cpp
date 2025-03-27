#include <cassert>
#include <functional>

#include "dbcon.hpp"
#include "Data/Public/user.hpp"

void TestDB()
{
    Database::Table* user_table = GlobalScope::GetDatabase().GetTable( "User" );
    assert( user_table );
    
    UserProfile test{
        "name", "test@test.com", std::vector<std::byte>{ 32, ( std::byte )0 }
    };
    user_table->Execute<bool>( &UserProfile::Insert, test );

    const auto& id = user_table->Execute<size_t>( &UserProfile::GetIdentifier, "name" );
    assert( id != -1 );
    CONSOLE_OUT( __FUNCTION__, "User {} registration success", id )
}
