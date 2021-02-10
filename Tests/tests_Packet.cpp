/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet unit tests
 */

#include <gtest/gtest.h>

#include <Protocol/Packet.hpp>
#include <Protocol/ConnectionProtocol.hpp>

using namespace Protocol;

TEST(Packet, SimpleIntPacket)
{
    char buff[sizeof(WritablePacket::Header) + sizeof(int)];
    int x = 42;

    // Serialize data
    WritablePacket wpacket(std::begin(buff), std::end(buff));
    wpacket.prepare(ProtocolType::Connection, ConnectionCommand::IDAssignment);
    ASSERT_EQ(wpacket.protocolType(), ProtocolType::Connection);
    ASSERT_EQ(wpacket.commandAs<ConnectionCommand>(), ConnectionCommand::IDAssignment);
    ASSERT_EQ(wpacket.payload(), 0u);
    ASSERT_EQ(wpacket.footprintStackSize(), 0u);
    ASSERT_EQ(wpacket.bytesAvailable(), sizeof(int));
    wpacket << x;
    ASSERT_EQ(wpacket.payload(), 4u);
    // Overflow test
    ASSERT_EQ(wpacket.bytesAvailable(), 0);
#if CORE_DEBUG_BUILD
    ASSERT_ANY_THROW(wpacket << x);
#endif

    // Deserialize data
    ReadablePacket rpacket(std::begin(buff), std::end(buff));
    ASSERT_EQ(rpacket.protocolType(), ProtocolType::Connection);
    ASSERT_EQ(rpacket.commandAs<ConnectionCommand>(), ConnectionCommand::IDAssignment);
    ASSERT_EQ(rpacket.payload(), 4u);
    ASSERT_EQ(rpacket.footprintStackSize(), 0u);
    ASSERT_EQ(rpacket.bytesAvailable(), 4);
    const auto y = rpacket.extract<int>();
    ASSERT_EQ(x, y);

    // Overflow test
    ASSERT_EQ(rpacket.bytesAvailable(), 0);
    ASSERT_ANY_THROW(static_cast<void>(rpacket.extract<int>()));
}

TEST(Packet, StringVector)
{
    constexpr char String1[] = "Hello";
    constexpr char String2[] = "0123456789";
    constexpr char String3[] = "abcdefghijklmnopqrstuvwxyz";
    constexpr auto PayloadSize =
            4 * sizeof(std::uint16_t) // 1 vector + 3 string payloads
            + sizeof(String1) + sizeof(String2) + sizeof(String3) // 3 strings in bytes
            - 3; // that aren't null terminated

    char buff[sizeof(WritablePacket::Header) + PayloadSize];
    std::vector<std::string> input {
        String1, String2, String3
    };

    ASSERT_EQ(input[0].size(), sizeof(String1) - 1);
    ASSERT_EQ(input[1].size(), sizeof(String2) - 1);
    ASSERT_EQ(input[2].size(), sizeof(String3) - 1);

    // Serialize data
    WritablePacket wpacket(std::begin(buff), std::end(buff));
    wpacket.prepare(ProtocolType::Connection, ConnectionCommand::IDAssignment);
    ASSERT_EQ(wpacket.protocolType(), ProtocolType::Connection);
    ASSERT_EQ(wpacket.commandAs<ConnectionCommand>(), ConnectionCommand::IDAssignment);
    ASSERT_EQ(wpacket.payload(), 0u);
    ASSERT_EQ(wpacket.footprintStackSize(), 0u);
    ASSERT_EQ(wpacket.bytesAvailable(), PayloadSize);
    wpacket << input;
    ASSERT_EQ(wpacket.payload(), PayloadSize);
    // Overflow test
    ASSERT_EQ(wpacket.bytesAvailable(), 0);
#if CORE_DEBUG_BUILD
    ASSERT_ANY_THROW(wpacket << 42);
#endif

    // Deserialize data
    ReadablePacket rpacket(std::begin(buff), std::end(buff));
    ASSERT_EQ(rpacket.protocolType(), ProtocolType::Connection);
    ASSERT_EQ(rpacket.commandAs<ConnectionCommand>(), ConnectionCommand::IDAssignment);
    ASSERT_EQ(rpacket.payload(), PayloadSize);
    ASSERT_EQ(rpacket.footprintStackSize(), 0u);
    ASSERT_EQ(rpacket.bytesAvailable(), PayloadSize);
    const auto output = rpacket.extract<std::vector<std::string>>();
    ASSERT_EQ(input, output);

    // Overflow test
    ASSERT_EQ(rpacket.bytesAvailable(), 0);
    ASSERT_ANY_THROW(static_cast<void>(rpacket.extract<int>()));
}

TEST(Packet, PacketWithFootprintStack)
{
    char buff[sizeof(WritablePacket::Header) + sizeof(int) + 2 * (sizeof(BoardID))];
    int x = 42;

    // Serialize data
    WritablePacket wpacket(std::begin(buff), std::end(buff));
    wpacket.prepare(ProtocolType::Connection, ConnectionCommand::IDAssignment);
    ASSERT_EQ(wpacket.protocolType(), ProtocolType::Connection);
    ASSERT_EQ(wpacket.commandAs<ConnectionCommand>(), ConnectionCommand::IDAssignment);
    ASSERT_EQ(wpacket.payload(), 0u);
    ASSERT_EQ(wpacket.footprintStackSize(), 0u);
    ASSERT_EQ(wpacket.bytesAvailable(), sizeof(int) + 2 * (sizeof(BoardID)));
    wpacket << x;
    ASSERT_EQ(wpacket.payload(), 4u);
    wpacket.pushFootprint(static_cast<BoardID>(42));
    wpacket.pushFootprint(static_cast<BoardID>(84));
    ASSERT_EQ(wpacket.payload(), 6u);
    ASSERT_EQ(wpacket.footprintStackSize(), 2u);

    // Overflow test
    ASSERT_EQ(wpacket.bytesAvailable(), 0);
#if CORE_DEBUG_BUILD
    ASSERT_ANY_THROW(wpacket << x);
#endif

    // Deserialize data
    ReadablePacket rpacket(std::begin(buff), std::end(buff));
    ASSERT_EQ(rpacket.protocolType(), ProtocolType::Connection);
    ASSERT_EQ(rpacket.commandAs<ConnectionCommand>(), ConnectionCommand::IDAssignment);
    ASSERT_EQ(rpacket.payload(), 6u);
    ASSERT_EQ(rpacket.footprintStackSize(), 2u);
    ASSERT_EQ(rpacket.bytesAvailable(), 4);
    const auto y = rpacket.extract<int>();
    ASSERT_EQ(x, y);

    // Overflow test
    ASSERT_EQ(rpacket.bytesAvailable(), 0);
    ASSERT_ANY_THROW(static_cast<void>(rpacket.extract<int>()));

    ASSERT_EQ(wpacket.popFrontStack(), 42);
    ASSERT_EQ(wpacket.payload(), 5u);
    ASSERT_EQ(wpacket.footprintStackSize(), 1u);
    ASSERT_EQ(wpacket.popFrontStack(), 84);
    ASSERT_EQ(wpacket.payload(), 4u);
    ASSERT_EQ(wpacket.footprintStackSize(), 0u);
}
