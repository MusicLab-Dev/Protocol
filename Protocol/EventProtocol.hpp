/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Connection protocol
 */

#pragma once

#include "Protocol.hpp"

namespace Protocol
{
    /** @brief Every commands of the Event protocol */
    DeclareProtocolEnum(EventCommand) {
        Update = 200u
    };
}