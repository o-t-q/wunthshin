#include "../Public/utility.hpp"

enum class eTableName
{
	Inventory,
	CharacterStatus,
	Max
};

Logger G_Logger{};

static std::string GetTableName(eTableName TableName)
{
    std::string tableName = "";

    switch ( TableName )
    {
        case eTableName::Inventory:
            tableName = "";
            break;
        case eTableName::CharacterStatus:
            tableName = "characterstatus";
            break;
        case eTableName::Max:
            break;
        default:
            break;
    }

    return tableName;
}
