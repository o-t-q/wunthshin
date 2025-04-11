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
    size_t GetLoginUser( const UUID& InSessionId ) const;

private:
    mutable std::atomic<bool>        m_mtx_;
    std::unordered_map<UUID, size_t> m_login_;

};