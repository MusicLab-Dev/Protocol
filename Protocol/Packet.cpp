/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet
 */

#include "Packet.hpp"

using namespace Protocol;

WritablePacket &WritablePacket::operator=(ReadablePacket &other) noexcept
{
    std::size_t otherPayload = other.payload();

    header()->magicKey = SpecialLabMagicKey;
    header()->protocolType = other.protocolType();
    header()->command = other.command();
    header()->payload = otherPayload;
    header()->footprintStackSize = other.footprintStackSize();

    std::memcpy(data(), other.data(), otherPayload);
    _writeIndex = otherPayload;

    return *this;
}

void WritablePacket::pushFootprint(const BoardID boardID)
{
    BoardID *stackEnd = data() + header()->payload;

    *stackEnd = boardID;
    header()->payload++;
    header()->footprintStackSize++;
    _writeIndex++;
}

BoardID WritablePacket::popFrontStack(void)
{
    if (header()->footprintStackSize == 0)
        return 0;
    BoardID *stackFront = data() + header()->payload - header()->footprintStackSize;
    BoardID *stackEnd = data() + header()->payload - 1;
    const BoardID first = *stackFront;
    header()->payload--;
    header()->footprintStackSize--;
    _writeIndex--;
    if (header()->footprintStackSize == 0) {
        *stackFront = 0;
        return first;
    }
    std::memmove(stackFront, stackFront + 1, footprintStackSize());
    *stackEnd = 0u;
    return first;
}

BoardID WritablePacket::popBackStack(void)
{
    BoardID *stackEnd = data() + header()->payload - 1;

    BoardID last = *stackEnd;
    *stackEnd = 0u;
    header()->payload--;
    header()->footprintStackSize--;
    _writeIndex--;
    return last;
}
