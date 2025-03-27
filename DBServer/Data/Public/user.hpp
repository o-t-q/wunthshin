#pragma once
#include <string>
#include <pqxx/pqxx>
#include "../../dbcon.hpp"

struct UserProfile
{
    std::string            name;
    std::string            email;
    std::vector<std::byte> hashedPassword;

    static bool   Insert( const UserProfile& table, pqxx::work&& tx );
    static size_t TryLogin( const std::string_view name, const std::array<std::byte, 32>& hashedPassword, pqxx::work&& tx  );
    static size_t GetIdentifier( const std::string_view name, pqxx::work&& tx );
};
