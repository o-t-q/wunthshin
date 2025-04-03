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
