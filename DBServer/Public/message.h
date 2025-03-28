#pragma once
#include <array>
#include <vector>
#include <type_traits>
#include <string>

enum class EMessageType : int32_t
{
    Unspecified,
	PingPong,
    Login,
    LoginStatus,
    Logout,
    LogoutOK,
    Register,
    RegisterStatus,
    MAX
};

template <EMessageType MessageType>
concept CMessageConstraint = ( size_t )MessageType >= 0 && ( size_t )MessageType < ( size_t )EMessageType::MAX;

enum class ERegistrationFailCode : int32_t
{
    None,
    Name,
    Email
};

constexpr size_t GetMaxMessageIndex()
{
    return static_cast<size_t>( EMessageType::MAX );
}

template <EMessageType MessageType>
    requires CMessageConstraint<MessageType>
struct MessageTypeResolver 
{
    using type = void;
};

#define DEFINE_MSG(NAME, ENUM) \
    struct NAME : MessageT<ENUM> {}; \
    template <> \
    struct MessageTypeResolver<ENUM> \
    { using type = NAME; };


#define DEFINE_MSG_WITH_BODY(NAME, ENUM, BODY) \
    struct NAME : MessageT<ENUM> \
    { BODY }; \
    template <> \
    struct MessageTypeResolver<ENUM> \
    { using type = NAME; };

#pragma pack( push, 1 )
struct MessageBase 
{
    explicit MessageBase( EMessageType InMessageType ) : messageType( InMessageType )
    { }

    MessageBase(MessageBase&& other) noexcept
    {
        messageType = other.messageType;
        other.messageType = EMessageType::Unspecified;
    }

    MessageBase(const MessageBase& other)
    {
        messageType = other.messageType;
    }

    EMessageType GetType() const
    {
        return messageType;
    }

private:
    EMessageType messageType;
};

template <EMessageType MessageType>
    requires CMessageConstraint<MessageType>
struct MessageT : MessageBase
{
    constexpr MessageT() : MessageBase( MessageType )
    {
    }

    MessageT( MessageT&& other ) noexcept : MessageBase( other ) {}
};

using Varchar = std::array<char, 255>;
using HashArray = std::array<std::byte, 32>;
using UUID = std::array<std::byte, 16>;

DEFINE_MSG( UnspecifiedMessage, EMessageType::Unspecified )
DEFINE_MSG( PingPongMessage, EMessageType::PingPong )
DEFINE_MSG_WITH_BODY( LoginMessage, EMessageType::Login, 
    Varchar name{}; 
    HashArray hashedPassword{}; )
DEFINE_MSG_WITH_BODY(
        LoginStatusMessage,
        EMessageType::LoginStatus,
    LoginStatusMessage( UUID&& inSessionId )
    {
        sessionId = inSessionId;
    }
    bool success = false;    
    UUID sessionId{};
);
DEFINE_MSG_WITH_BODY(
        LogoutMessage, EMessageType::Logout, 
    LogoutMessage( UUID&& inSessionId ) : sessionId( inSessionId ) {}
    UUID sessionId{};
)
DEFINE_MSG_WITH_BODY(
        LogoutOKMessage, EMessageType::LogoutOK,
           LogoutOKMessage(bool flag) : success(flag) {}
           bool success; )
DEFINE_MSG_WITH_BODY( RegisterMessage, EMessageType::Register,
           std::string name; std::string email; HashArray hashedPassword{}; )
DEFINE_MSG_WITH_BODY(
        RegisterStatusMessage, EMessageType::RegisterStatus,
           bool success = false; ERegistrationFailCode code = ERegistrationFailCode::None;
           RegisterStatusMessage() = default;
           RegisterStatusMessage(bool inSuccess, ERegistrationFailCode inFailCode)
           {
               success = inSuccess;
               code = inFailCode;
           } )
#pragma pack( pop )

template <size_t... Values>
constexpr auto resolve_message_sizes_impl( std::integer_sequence<size_t, Values...> int_seq )
{
    std::array<size_t, GetMaxMessageIndex()> out  = {};
    const std::vector<size_t>                temp = { sizeof( typename MessageTypeResolver<static_cast<EMessageType>( Values )>::type )... };
    for ( int i = 0; i < GetMaxMessageIndex(); ++i )
    {
        out.at( i ) = temp.at( i );
    }
    return out;
}

constexpr auto resolve_message_sizes()
{
    using indices = std::make_index_sequence<GetMaxMessageIndex()>;
    return resolve_message_sizes_impl( indices{} );
}

static constexpr auto G_MessageSize = []() { return resolve_message_sizes(); }();
