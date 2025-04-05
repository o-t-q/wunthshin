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
    { \
        NAME() = default; \
        BODY \
    }; \

#pragma pack( push, 1 )
struct MessageBase 
{
    ~MessageBase() = default;
    explicit MessageBase( EMessageType InMessageType ) : messageType{ InMessageType }
    { }
    MessageBase(const MessageBase&) = delete;
    MessageBase& operator=(const MessageBase&) = delete;
    MessageBase& operator=(MessageBase&& other) noexcept
    {
        messageType = other.messageType;
        other.messageType = EMessageType::Unspecified;
        return *this;
    }
    MessageBase(MessageBase&& other) noexcept
    {
        messageType = other.messageType;
        other.messageType = EMessageType::Unspecified;
    }

    [[nodiscard]] EMessageType GetType() const
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
    static constexpr EMessageType message_type = MessageType;

    ~MessageT() = default;
    MessageT() : MessageBase{ MessageType }
    { }
    MessageT(const MessageT&) = delete;
    MessageT& operator=(const MessageT&) = delete;
    MessageT( MessageT&& other ) noexcept : MessageBase( std::move( other ) )
    { }
    MessageT& operator=( MessageT&& other ) noexcept
    {
        MessageBase::operator=( std::move( other ) );
        return *this;
    }
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
LoginStatusMessage(const bool InSuccess, const UUID& InSessionId)
{
    success = InSuccess;
    sessionId = InSessionId;
}
bool success = false;
UUID sessionId{};)

DEFINE_MSG_WITH_BODY(
        LogoutMessage,
        EMessageType::Logout,
        EMessageChannelType::Login,
LogoutMessage(const UUID& InSessionId)
{
    sessionId = InSessionId;
}
UUID sessionId{};)

DEFINE_MSG_WITH_BODY(
        LogoutOKMessage,
        EMessageType::LogoutOK,
        EMessageChannelType::Login,
LogoutOKMessage(const bool InSuccess)
{
    success = InSuccess;
}
bool success = false;)

DEFINE_MSG_WITH_BODY( RegisterMessage, EMessageType::Register, EMessageChannelType::Register, 
    Varchar name{}; Varchar email{}; HashArray hashedPassword{};
)

DEFINE_MSG_WITH_BODY(
        RegisterStatusMessage, EMessageType::RegisterStatus, EMessageChannelType::Register,

RegisterStatusMessage(bool InSucess, ERegistrationFailCode InCode)
{
    success = InSucess;
    code = InCode;
}
bool success = false; 
ERegistrationFailCode code = ERegistrationFailCode::None;)

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
