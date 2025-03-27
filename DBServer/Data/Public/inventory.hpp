#pragma once
#include <string>
#include <pqxx/pqxx>

#include "item.hpp"
#include "../../dbcon.hpp"
#include "../../DBServer.h"

struct Inventory
{
    size_t count;
	std::vector<size_t> itemList;
	std::vector<size_t> itemCount;

    static bool Insert( const size_t owner, pqxx::work&& tx)
    {
        const pqxx::result result = tx.exec( "INSERT INTO inventory VALUES ($1, DEFAULT, DEFAULT)", pqxx::params{ owner } );
        const size_t row_count = result.affected_rows();
        tx.commit();
        return row_count;
    }

	static bool   NewItem( const size_t owner, const std::string_view item_name, const size_t count, pqxx::work&& tx )
    {
        const Database::Table* item_table = GlobalScope::GetDatabase().GetTable( "Item" );
        const auto             new_item_id    = item_table->Execute<size_t>( &Item::GetIdentifier, item_name  );

        // shouldn't be happening due to throw from the pqxx
        if ( new_item_id == 0 )
        {
            return false;
        }

        std::vector<size_t> newItemList;
        std::vector<size_t> newItemCount;

        const pqxx::result inventory_query = tx.exec( "SELECT item_id, count FROM inventory WHERE owner=$1", pqxx::params{ owner } );

        const pqxx::row user_row       = inventory_query.one_row();
        const auto&     itemArray      = user_row["item_id"].as_sql_array<size_t>();
        const auto&     itemCountArray = user_row["item_count"].as_sql_array<size_t>();
        for ( size_t i = 0; i < itemArray.size(); ++i )
        {
            newItemList.emplace_back( itemArray[i] );
        }
        for ( size_t i = 0; i < itemCountArray.size(); ++i )
        {
            newItemCount.emplace_back( itemCountArray[i] );
        }
        newItemList.emplace_back( new_item_id );
        newItemCount.emplace_back( count );

        const pqxx::result result = tx.exec( "UPDATE users SET (item=$1, count=$2) WHERE owner=$3", { newItemList, newItemCount, owner } );
        const size_t row_count = result.affected_rows();
        tx.commit();
        return row_count;
    }
};

static TableRegistration<Inventory> InventoryTableRegistration( "Inventory" );