#pragma once
#include <string>
#include <pqxx/pqxx>

#include "../../Public/dbcon.hpp"
#include "../../Public/DBServer.h"
#include "../../Public/message.h"

struct Characterstatus
{
    static constexpr const char* TableName = "characterstatus";

    size_t user_id; 
    size_t character_id;
    size_t current_hp;
    size_t current_exp;
    
    static bool Insert(const Characterstatus& table, pqxx::work&& tx)
    {
        const pqxx::result result   = tx.exec( "INSERT INTO $1 VALUES (DEFAULT, $2, $3, $4, $5)",
                         { TableName, table.user_id, table.character_id, table.current_hp, table.current_exp } );
        const size_t rowCount = result.affected_rows();
        tx.commit();
        return rowCount;
    }

    static bool FindByUserID( const size_t user_id, Characterstatus& out_status, pqxx::work&& tx )
    {
        const pqxx::result result    = tx.exec( "SELECT FROM $1 WHERE user_id=$2", pqxx::params( TableName, user_id ) );
        const pqxx::row    resultRow = result.one_row();

        out_status.user_id      = resultRow[ "user_id" ].as<size_t>();
        out_status.character_id = resultRow[ "character_id" ].as<size_t>();
        out_status.current_hp   = resultRow[ "current_hp" ].as<size_t>();
        out_status.current_exp  = resultRow[ "current_exp" ].as<size_t>();
        return result.size();
    }

    static bool UpdateValue(const Characterstatus& value, pqxx::work&& tx)
    {
        const pqxx::result result =
                tx.exec( "UPDATE characterstatus SET current_hp=$1, current_exp=$2 WHERE user_id=$3, character_id=$4",
                         {
                                 value.current_hp,
                                 value.current_exp,
                                 value.user_id,
                                 value.character_id,
                         } );
        tx.commit();
        return result.affected_rows();
    }
};

static TableRegistration<Characterstatus> CharacterStatusRegistration( "characterstatus" );
