/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Control
 */

#pragma once

#include <Core/Utils.hpp>

namespace Protocol
{
    /** @brief Type of module */
    enum class ModuleType : std::uint8_t {
        None,
        Button,
        TwoButtons,
        ThreeButtons,
        Potentiometer,
        PotentiometerButton,
    };

    /** @brief Describe a module parameter data */
    struct Module
    {
        ModuleType type { ModuleType::None };
        std::uint8_t value1 { 0u };
        std::uint8_t value2 { 0u };
        std::uint8_t value3 { 0u };
    };

#pragma pack(push, 1)
    /** @brief Describe a module that has been connected to the board */
    struct ModuleIdentity
    {
        ModuleType type { ModuleType::None };
        std::uint8_t index { 0u };
        std::uint8_t value1 { 0u };
        std::uint8_t value2 { 0u };
        std::uint8_t value3 { 0u };
    };

    /** @brief Describe a module that has been connected to the board */
    struct ModuleUpdate
    {
        std::uint8_t index { 0u };
        std::uint8_t value1 { 0u };
        std::uint8_t value2 { 0u };
        std::uint8_t value3 { 0u };
    };
#pragma pack(pop)
}