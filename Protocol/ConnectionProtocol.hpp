/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Connection protocol
 */

#pragma once

#include "Protocol.hpp"

namespace Protocol
{
    /** @brief Every commands of the Connection protocol */
    DeclareProtocolEnum(ConnectionCommand) {
        Connect = 100u,
        Update
    };
}