#pragma once
#include <string>
#include <pqxx/pqxx>

#include "item.hpp"
#include "../../Public/dbcon.hpp"
#include "../../Public/DBServer.h"

struct Inventory
{
    size_t count;
	std::vector<size_t> itemList;
	std::vector<size_t> itemCount;

    static bool Insert( const size_t owner, pqxx::work&& tx )
    {
        const pqxx::result result = tx.exec( "INSERT INTO inventory VALUES ($1, array[]::bigint[], array[]::bigint[])", pqxx::params{ owner } );
        const size_t row_count = result.affected_rows();
        tx.commit();
        return row_count;
    }

    static bool GetAllItems(const size_t owner, const size_t page, bool& end, ItemArray& out_array, pqxx::work&& tx)
    {
        out_array = {};

        const Database::Table* inventory_table = GlobalScope::GetDatabase().GetTable( "inventory" );
        const pqxx::result     inventory_query =
                tx.exec( "SELECT item_id, item_count FROM inventory WHERE owner=$1", pqxx::params{ owner } );

        const pqxx::row user_row       = inventory_query.one_row();
        const auto&     itemArray      = user_row[ "item_id" ].as_sql_array<size_t>();
        const auto&     itemCountArray = user_row[ "item_count" ].as_sql_array<size_t>();

        if ( itemArray.size() != itemCountArray.size() )
        {
            CONSOLE_OUT( __FUNCTION__, "User {} inventory seems invalid, no returing the items", owner );
            return false;
        }

        auto itemIterator = itemArray.cbegin() + (page * 100);
        auto itemCountIterator = itemCountArray.cbegin() + ( page * 100 );

        if (itemIterator > itemArray.cend())
        {
            CONSOLE_OUT( __FUNCTION__, "User {} requested inventory out of index", owner );
            return false;
        }

        end              = false;
        size_t iteration = 0;
        while ( iteration < 100 )
        {
            if ( itemIterator >= itemArray.cend() )
            {
                end = true;
                break;
            }

            out_array[ iteration ].itemID = *itemIterator;
            out_array[ iteration ].count  = *itemCountIterator;

            iteration++;
            ++itemIterator;
            ++itemCountIterator;
        }

        return true;
    }

	static bool   NewItem( const size_t owner, const size_t item_id, const size_t count, pqxx::work&& tx )
    {
        const Database::Table* item_table = GlobalScope::GetDatabase().GetTable( "Item" );
        const auto&            returnValue = item_table->ExecuteChild<bool>( std::move( tx ), &Item::Find, item_id );
        if ( !returnValue.first )
        {
            CONSOLE_OUT( __FUNCTION__, "Requested unknown item {}", item_id );
            return false;
        }

        std::vector<size_t> newItemList;
        std::vector<size_t> newItemCount;

        const pqxx::result inventory_query = returnValue.second.exec( "SELECT item_id, item_count FROM inventory WHERE owner=$1", pqxx::params{ owner } );

        const pqxx::row user_row       = inventory_query.one_row();
        const auto&     itemArray      = user_row["item_id"].as_sql_array<size_t>();
        const auto&     itemCountArray = user_row["item_count"].as_sql_array<size_t>();
       
        bool   exist = false;
        size_t index = -1;

        for ( size_t i = 0; i < itemArray.size(); ++i )
        {
            if (itemArray[i] == item_id)
            {
                exist = true;
                index = i;
            }
            newItemList.emplace_back( itemArray[i] );
        }
        for ( size_t i = 0; i < itemCountArray.size(); ++i )
        {
            newItemCount.emplace_back( itemCountArray[i] );
        }

        if ( !exist )
        {
            newItemList.emplace_back( item_id );
            newItemCount.emplace_back( count );
        }
        else
        {
            newItemCount[ index ] += count;
        }

        const pqxx::result result    = returnValue.second.exec( "UPDATE inventory SET item_id=$1, item_count=$2 WHERE owner=$3",
                                                                { newItemList, newItemCount, owner } );
        const size_t row_count = result.affected_rows();
        returnValue.second.commit();
        return row_count;
    }
};

static TableRegistration<Inventory> InventoryTableRegistration( "inventory" );