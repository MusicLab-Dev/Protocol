/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet unit tests
 */

#include <gtest/gtest.h>

#include <Protocol/Packet.hpp>

using namespace Protocol;

TEST(Packet, InitDestroy)
{
    Packet packet;
    ASSERT_EQ(packet.foo(), 32);
    *packet.data() = 1;
    ASSERT_EQ(packet.foo(), 42);
}