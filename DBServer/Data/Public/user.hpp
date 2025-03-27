#pragma once
#include <string>

struct UserProfile
{
    size_t      identifier;
    std::string name;
    std::string email;
    std::string hashedPassword;
};