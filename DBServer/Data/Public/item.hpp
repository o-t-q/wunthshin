#pragma once
#include <string>
#include <pqxx/pqxx>

#include "../../Public/dbcon.hpp"

struct Item
{
    uint32_t ID;
    Varchar name;

    static bool Insert( const Item& item, pqxx::work&& tx )
    {
        std::string_view   strView( item.name.data() );
        const pqxx::result result    = tx.exec( "INSERT INTO items VALUES ($1, $2);", pqxx::params{ item.ID, strView } );
        const size_t row_count = result.affected_rows();
        tx.commit();
        return row_count;
    }

    static bool Find( const size_t id, pqxx::work&& tx )
    {
        const pqxx::result result = tx.exec( "SELECT FROM items WHERE id=$1", pqxx::params{ id } );
        return result.size();
    }

    static size_t GetIdentifier( const std::string_view name, pqxx::work&& tx )
    {
        pqxx::params params;
        params.append( name );
        return tx.query_value<size_t>( "SELECT id FROM items WHERE name=$1;", params);
    }
};

struct Weapon
{
    uint32_t ID;
    Varchar  name;

    static bool Insert( const Item& item, pqxx::work&& tx )
    {
        std::string_view   strView( item.name.data() );
        const pqxx::result result = tx.exec( "INSERT INTO weapons VALUES ($1, $2);", pqxx::params{ item.ID, strView } );
        const size_t       row_count = result.affected_rows();
        tx.commit();
        return row_count;
    }

    static bool Find( const size_t id, pqxx::work&& tx )
    {
        const pqxx::result result = tx.exec( "SELECT FROM weapons WHERE id=$1", pqxx::params{ id } );
        return result.size();
    }

    static size_t GetIdentifier( const std::string_view name, pqxx::work&& tx )
    {
        pqxx::params params;
        params.append( name );
        return tx.query_value<size_t>( "SELECT id FROM weapons WHERE name=$1;", params );
    }
};

static TableRegistration<Item> ItemTableRegistration( "items" );
static TableRegistration<Weapon> WeaponTableRegistration( "weapons" );
