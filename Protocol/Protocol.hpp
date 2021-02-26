/**
 * @ Author: Paul Creze
 * @ Description: Protocol common types
 */

#pragma once

#include <cstdint>
#include <utility>

#include "Module.hpp"

#define DeclareProtocolEnum(Name) enum class Name : Protocol::Command

namespace Protocol
{
    /** @brief Distance with the studio */
    using NodeDistance = std::uint8_t;

    /** @brief Distance between two nodes */
    using BoardID = std::uint8_t;

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
};