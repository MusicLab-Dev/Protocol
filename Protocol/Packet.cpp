/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet
 */

#include "Packet.hpp"

using namespace Protocol;

int Packet::foo() {
    if (*_data)
        return 42;
    return 32;
}