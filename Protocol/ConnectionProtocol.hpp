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
        /** @brief Initialize connection and assign a board ID
         *
         * -> Client:
         * @param void
         *
         * <- Server:
         * @param BoardID The assigned board ID
         */
        IDAssignment,

        /** @brief Share hardare specifications of the board
         *
         * -> Server:
         * @param void
         *
         * <- Client:
         * @param BoardSize Hardware board size
        */
        HardwareSpecs,
    };
}
