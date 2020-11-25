/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet
 */

#pragma once

#include <Core/Assert.hpp>

#include "Protocol.hpp"

namespace Protocol
{
    class ReadablePacket;
    class WritablePacket;

    namespace Internal
    {
        class PacketBase;
    }

    static_assert_sizeof(Header, 12);

    using MagicKey = std::uint32_t;

    /** @brief MusicLabMagicKey network packets magic key */
    constexpr MagicKey MusicLabMagicKey = 420;

    /** @brief Discovery packet */
    struct DiscoveryPacket
    {
        MagicKey magicKey { MusicLabMagicKey };
        BoardID boardID { 0u };
        ConnectionType connectionType { ConnectionType::None };
        NodeDistance distance { 0u };
    };
}

/** @brief Base of any packet */
class alignas_eighth_cacheline Protocol::Internal::PacketBase
{
public:
    using Payload = std::uint16_t;

    /** @brief Header of a packet */
    struct alignas(4) Header
    {
        MagicKey magicKey { MusicLabMagicKey };
        ProtocolType protocolType { ProtocolType::Connection };
        Command command { 0u };
        Payload payload { 0u };
        std::uint16_t footprintStack { 0u };
    };

    static_assert_sizeof(Header, 12);

    /** @brief Construct a packet from binary data */
    template<typename BinaryData>
    PacketBase(const BinaryData * const data) noexcept
        :   _header(reinterpret_cast<const Header * const>(data)) {}

    /** @brief Move constructor */
    PacketBase(PacketBase &&other) noexcept = default;

    /** @brief Destructor */
    ~PacketBase(void) noexcept = default;

    /** @brief Move assignment */
    PacketBase &operator=(PacketBase &&other) noexcept = default;


    /** @brief Fast boolean check */
    [[nodiscard]] operator bool(void) const noexcept { return _header; }


    /** @brief Get the packet payload (data size without header) */
    [[nodiscard]] Payload payload(void) const noexcept { return _header->payload; }

    /** @brief Get the total packet size (header and data) */
    [[nodiscard]] std::size_t totalSize(void) const noexcept { return payload() + sizeof(Header); }


    /** @brief Get the protocol type (Connection / Event) */
    [[nodiscard]] ProtocolType protocolType(void) const noexcept { return _header->protocolType; }

    /** @brief Get the packet opaque command */
    [[nodiscard]] Command command(void) const noexcept { return _header->command; }

    /** @brief Get the command casted to a specific type */
    template<typename CommandType>
    [[nodiscard]] CommandType commandAs(void) const noexcept { return static_cast<CommandType>(command()); }

    /** @brief Get the total packet size (header and data) */
    template<typename Type = std::uint8_t>
    [[nodiscard]] const Type *data(void) const noexcept { return reinterpret_cast<const Type *>(_header + 1); }


    /** @brief Get the packet payload (data size without header) */
    [[nodiscard]] std::uint16_t footprintStack(void) const noexcept { return _header->footprintStack; }

    /** @brief Get the begin pointer of footprint */
    [[nodiscard]] const BoardID *footprintBegin(void) const noexcept
        { return data() + _header->payload - _header->footprintStack; }

    /** @brief Get the end pointer of footprint */
    [[nodiscard]] const BoardID *footprintEnd(void) const noexcept
        { return data() + _header->payload; }

private:
    const Header *_header { nullptr };
};

static_assert_fit_eighth_cacheline(Protocol::Internal::PacketBase);

/** @brief A read-only packet */
class Protocol::ReadablePacket : public Internal::PacketBase
{
public:

private:
    std::size_t _readIndex { 0u };
};

/** @brief A write-only packet */
class Protocol::WritablePacket : public Internal::PacketBase
{
public:
    using Internal::PacketBase::PacketBase;
    using Internal::PacketBase::operator=;

    /** @brief Prepare a packet */
    template<typename CommandType, std::enable_if_t<sizeof(CommandType) == sizeof(Command)>* = nullptr>
    WritablePacket &prepare(const ProtocolType protocolType, const CommandType command);

    /** @brief Insert data to the packet */
    template<typename Type>
    WritablePacket &operator<<(Type &&other) noexcept_ndebug;

private:
    std::size_t _writeIndex { 0u };
    std::size_t _capacity { 0u };
};

#include "Packet.ipp"