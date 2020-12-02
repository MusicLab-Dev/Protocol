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
        /**
         * @brief Initialize connection
         *  This is a long description ....
         *  ..........................
         *
         * @param BoardID The board that receive XYZ
         */
        IDRequest = 100u,
        IDAssignement
    };
}