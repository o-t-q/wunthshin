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

enum class EMessageChannelType : int32_t
{
    Unspecified,
    Comm,
    Login,
    Register,
    MAX
};

template <EMessageType MessageType>
concept CMessageConstraintSimple = ( int32_t )MessageType >= 0 && ( int32_t )MessageType < ( size_t )EMessageType::MAX;

template <EMessageType MessageType>
    requires CMessageConstraintSimple<MessageType>
struct MessageTypeResolver;

template <EMessageType MessageType>
concept CMessageConstraint =
        CMessageConstraintSimple<MessageType> && !std::is_same_v<typename MessageTypeResolver<MessageType>::type, void>;

template <EMessageChannelType ChannelType>
concept CChannelConstraintSimple =
        ( int32_t )ChannelType >= 0 && ( int32_t )ChannelType < ( int32_t )EMessageChannelType::MAX;

template <EMessageType MessageType>
    requires CMessageConstraint<MessageType>
struct MessageChannelTypeResolver;


enum class ERegistrationFailCode : uint8_t
{
    None,
    Name,
    Email
};

constexpr size_t GetMaxMessageIndex()
{
    return static_cast<size_t>( EMessageType::MAX );
}

#define DEFINE_MSG(NAME, MSGTYPE, CHLTYPE) DEFINE_MSG_WITH_BODY(NAME, MSGTYPE, CHLTYPE, )

#define DEFINE_MSG_WITH_BODY( NAME, MSGTYPE, CHLTYPE, BODY ) \
    struct NAME; \
    template <> \
    struct MessageTypeResolver<MSGTYPE> \
    { using type = NAME; }; \
    template <> \
    struct MessageChannelTypeResolver<MSGTYPE> \
    { \
        static_assert(CChannelConstraintSimple<CHLTYPE>); \
        constexpr static EMessageChannelType value = CHLTYPE; \
    }; \
    struct NAME : MessageT<MSGTYPE> \
    { BODY };

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

using Varchar = std::array<char, 256>;
using HashArray = std::array<std::byte, 32>;
using UUID = std::array<std::byte, 16>;

DEFINE_MSG( UnspecifiedMessage, EMessageType::Unspecified, EMessageChannelType::Unspecified )
DEFINE_MSG( PingPongMessage, EMessageType::PingPong, EMessageChannelType::Comm )
DEFINE_MSG_WITH_BODY( LoginMessage, EMessageType::Login, EMessageChannelType::Login,
    Varchar name{}; 
    HashArray hashedPassword{}; )
DEFINE_MSG_WITH_BODY(
        LoginStatusMessage,
        EMessageType::LoginStatus,
        EMessageChannelType::Login,
    LoginStatusMessage( UUID&& inSessionId )
    {
        sessionId = inSessionId;
    }
    bool success = false;    
    UUID sessionId{};
);
DEFINE_MSG_WITH_BODY(
        LogoutMessage,
        EMessageType::Logout,
        EMessageChannelType::Login,
    LogoutMessage( UUID&& inSessionId ) : sessionId( inSessionId ) {}
    UUID sessionId{};
)
DEFINE_MSG_WITH_BODY(
        LogoutOKMessage,
        EMessageType::LogoutOK,
        EMessageChannelType::Login,
           LogoutOKMessage(bool flag) : success(flag) {}
           bool success; )
DEFINE_MSG_WITH_BODY( RegisterMessage, EMessageType::Register, EMessageChannelType::Register,
           std::string name; std::string email; HashArray hashedPassword{}; )
DEFINE_MSG_WITH_BODY(
        RegisterStatusMessage, EMessageType::RegisterStatus, EMessageChannelType::Register,
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

template <size_t... Values>
constexpr auto resolve_message_channels_impl( std::integer_sequence<size_t, Values...> int_seq )
{
    std::array<size_t, GetMaxMessageIndex()> out = {};
    const std::vector<size_t> temp = { (size_t)MessageChannelTypeResolver<static_cast<EMessageType>( Values )>::value... };
    for (int i = 0; i < GetMaxMessageIndex(); ++i)
    {
        out.at( i ) = temp.at( i );
    }
    return out;
}

constexpr auto resolve_message_channels()
{
    using indices = std::make_index_sequence<GetMaxMessageIndex()>;
    return resolve_message_channels_impl( indices{} );
}

static constexpr auto G_MessageSize = []() { return resolve_message_sizes(); }();
static constexpr auto G_MessageChannels = []() { return resolve_message_channels(); }();
