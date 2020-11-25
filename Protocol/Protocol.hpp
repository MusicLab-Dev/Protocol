/**
 * @ Author: Paul Creze
 * @ Description: Protocol common types
 */

#pragma once

#include <cstdint>

namespace Protocol
{
    /** @brief Distance with the studio */
    using NodeDistance = std::uint8_t;

    /** @brief Distance between two nodes */
    using BoardID = std::uint16_t;

    /** @brief Hardware connection type */
    enum class ConnectionType : std::uint8_t {
        None,
        USB,
        WIFI
    };

    /** @brief Describe a parameter data */
    struct Control
    {
        /** @brief Type of control */
        enum class Type : std::uint8_t {
            None,
            Button,
            Potentiometer
        };

        Type type { Type::None };
        std::uint8_t value1 { 0u };
        std::uint8_t value2 { 0u };
        std::uint8_t value3 { 0u };
    };

    using MagicKey = std::uint32_t;
    using Payload = std::uint16_t;

    enum class PacketID : std::uint8_t {
        None,
        IDResquest,
        IDAssignment
    };

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

    struct PacketHeader
    {
        MagicKey magicKey { MusicLabMagicKey };
        PacketID packetID { PacketID::None };
        Payload payload { 0u };
    };

    class Packet
    {
    public:
        Packet(void) noexcept = default;
        Packet(PacketID id) noexcept { _header.packetID = id; };

        ~Packet(void) = default;

        void setPayload(std::size_t payload) noexcept { _header.payload = payload; }
        [[nodiscard]] std::uint8_t *data(void) noexcept { return reinterpret_cast<std::uint8_t *>(&_data); }
        [[nodiscard]] std::size_t size(void) const noexcept { return sizeof(PacketHeader) + _header.payload; }

    private:
        PacketHeader _header;
        std::uint8_t _data[256];
    };
};