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
        /** @brief Notify that controls has been connected
         *
         * -> Client:
         * @param Vector Contains the data of each input
         *  @param ControlType Type of the input
         *  @param ControlIndex The position of the control in the board
         *  @param State The state of the control
         */
        ControlsConnection = 200u,

        /** @brief Notify that controls has been disconnected
         *
         * -> Client:
         * @param Vector Contains the data of each control
         *  @param ControlIndex The position of the control in the board
         */
        ControlsDisconnected,

        /** @brief Notify that controls' states has changed
         *
         * -> Client:
         * @param Vector Contains the data of each control
         *  @param InputEvent The input event
         */
        ControlsChanged
    };
}
