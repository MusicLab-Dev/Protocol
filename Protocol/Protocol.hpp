/**
 * @ Author: Paul Creze
 * @ Description: Protocol common types
 */

#pragma once

#include <cstdint>
#include <utility>

#include "Control.hpp"

#define DeclareProtocolEnum(Name) enum class Name : Protocol::Command

namespace Protocol
{
    /** @brief Distance with the studio */
    using NodeDistance = std::uint8_t;

    /** @brief Distance between two nodes */
    using BoardID = std::uint8_t;

    /** @brief The hardware size of a board */
    struct BoardSize
    {
        std::uint16_t width { 0u };
        std::uint16_t heigth { 0u };
    };

    /** @brief Hardware connection type */
    enum class ConnectionType : std::uint8_t {
        None,
        USB,
        WIFI
    };

    /** @brief An opaque protocol command */
    using Command = std::uint16_t;

    /** @brief All types of protocols */
    DeclareProtocolEnum(ProtocolType) {
        Connection = 10u,
        Event
    };

    /** @brief Packet magic key type */
    using MagicKey = std::uint32_t;

    /** @brief Payload range */
    using Payload = std::uint16_t;

    /** @brief SpecialLabMagicKey network packets magic key */
    constexpr MagicKey SpecialLabMagicKey = 420 /* 0xABCDEF */;

    /** @brief Discovery packet */
    struct DiscoveryPacket
    {
        MagicKey magicKey;
        BoardID boardID;
        ConnectionType connectionType;
        NodeDistance distance;
    };

    /** @brief Describe a board */
    struct BoardDescriptor
    {
        std::uint8_t width {};
        std::uint8_t height {};
    };
}
