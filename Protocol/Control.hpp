/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Control
 */

#pragma once

#include <Core/Utils.hpp>

namespace Protocol
{
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
}