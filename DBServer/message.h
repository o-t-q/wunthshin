#pragma once
#include <optional>
#include <type_traits>
#include <magic_enum/magic_enum.hpp>

enum class EMessageType : int32_t
{
    Unspecified,
	PingPong,
    MAX
};

constexpr size_t GetMaxMessageIndex()
{
    return static_cast<size_t>( EMessageType::MAX );
}

#pragma pack( push, 1 )
struct MessageBase 
{
    MessageBase( EMessageType InMessageType ) : messageType( InMessageType )
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

template <EMessageType MessageType, typename Test = std::integral_constant<int32_t, static_cast<int32_t>(MessageType)>>
    requires( Test::value >= 0 && Test::value < GetMaxMessageIndex() )
struct MessageT : MessageBase
{
    constexpr MessageT() : MessageBase( MessageType )
    {
    }

    MessageT( MessageT&& other ) noexcept : MessageBase( other ) {}
};

struct UnspecifiedMessage : MessageT<EMessageType::Unspecified>
{ };
struct PingPongMessage : MessageT<EMessageType::PingPong> 
{ };
#pragma pack( pop )

template <size_t... Values>
constexpr auto resolve_message_sizes_impl( std::integer_sequence<size_t, Values...> int_seq )
{
    std::array<size_t, GetMaxMessageIndex()> out  = {};
    std::vector<size_t> temp = { sizeof( MessageT<static_cast<EMessageType>( Values )> )... };
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
