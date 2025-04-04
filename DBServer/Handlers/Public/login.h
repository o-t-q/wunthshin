#pragma once
#include <unordered_map>
#include "../../Public/message-handler.h"
#include <boost/container_hash/hash.hpp>

template <>
struct std::hash<UUID>
{
    size_t operator()(const UUID& value) const noexcept
    {
        static boost::hash<UUID> hasher;
        return hasher( value );
    }
};

struct LoginHandler : HandlerImplementation
{
    bool ShouldHandle( EMessageType messageType ) override;
    void Handle( const size_t index, MessageBase& message ) override;

private:
    std::unordered_map<UUID, size_t, std::hash<UUID>, std::equal_to<UUID>, boost::fast_pool_allocator<std::pair<const UUID, size_t>>> m_login_;

};