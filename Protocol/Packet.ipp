/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet
 */

#include <stdexcept>

using namespace Protocol;

template<typename BinaryData>
inline Internal::PacketBase::PacketBase(const BinaryData * const begin, [[maybe_unused]] const BinaryData * const end) noexcept_ndebug
    : _header(reinterpret_cast<const Header * const>(begin))
{
    coreAssert(static_cast<std::size_t>(std::distance(begin, end)) >= sizeof(Header),
        throw std::logic_error("Protocol::PacketBase::PacketBase: Invalid packet data size must be equal or greater than header size"));
}

template<typename Type>
inline Type ReadablePacket::extract(void)
{
    Type value;

    *this >> value;
    return value;
}

// !!!! TO FIX: WRONG COMMENT INSIDE !!!! //
template<typename OutputIterator>
void ReadablePacket::extract(const OutputIterator begin, const OutputIterator end)
{
    using Type = decltype(*std::declval<OutputIterator>());

    const auto size = static_cast<Payload>(std::distance(begin, end));
    const auto sizeInBytes = size * sizeof(Type);

    // Check if the container contains optimizable trivially copyable types
    if constexpr (Internal::IsTriviallyRangeCopyable<OutputIterator>) {
        if (bytesAvailable() < size * sizeof(Type))
            throw std::runtime_error("Protocol::WritablePacket::insert: Write overflow");
        std::memcpy(
            begin,
            currentDataHead(),
            sizeInBytes
        );
        _readIndex += sizeInBytes;
    } else {
        for (auto it = begin; it < end; ++it)
            *this >> *it;
    }
}

template<typename Container, Protocol::EnableIfContainerDetected<Container>*>
inline ReadablePacket &Protocol::ReadablePacket::operator>>(Container &container)
{
    container.resize(extract<Payload>());
    extract(std::begin(container), std::end(container));
    return *this;
}

template<typename Type, Protocol::EnableIfContainerNotDetected<Type>*>
inline ReadablePacket &Protocol::ReadablePacket::operator>>(Type &value)
{
    if (bytesAvailable() < sizeof(Type))
        throw std::runtime_error("Protocol::ReadablePacket::operator>>: Read overflow");
    value = *currentDataHead<Type>();
    _readIndex += sizeof(Type);
    return *this;
}


template<typename CommandType, std::enable_if_t<sizeof(CommandType) == sizeof(Protocol::Command)>*>
inline WritablePacket &Protocol::WritablePacket::prepare(const ProtocolType protocolType, const CommandType command)
{
    _writeIndex = 0u;
    header()->magicKey = SpecialLabMagicKey;
    header()->protocolType = protocolType;
    header()->command = static_cast<Command>(command);
    header()->payload = 0u;
    header()->footprintStackSize = 0u;
    return *this;
}

template<typename InputIterator>
inline WritablePacket &WritablePacket::insert(const InputIterator begin, const InputIterator end) noexcept_ndebug
{
    using Type = decltype(*std::declval<InputIterator>());

    const auto size = static_cast<Payload>(std::distance(begin, end));
    const auto sizeInBytes = size * sizeof(Type);

    // Check if the container contains optimizable trivially copyable types
    if constexpr (Internal::IsTriviallyRangeCopyable<InputIterator>) {
        coreAssert(bytesAvailable() >= size * sizeof(Type),
            throw std::runtime_error("WritablePacket::insert: Write overflow"));
        header()->payload += sizeInBytes;
        std::memcpy(
            currentDataHead(),
            begin,
            sizeInBytes
        );
        _writeIndex += sizeInBytes;
    } else {
        for (auto it = begin; it < end; ++it)
            *this << *it;
    }
    return *this;
}

template<typename Container, EnableIfContainerDetected<Container>*>
inline WritablePacket &WritablePacket::operator<<(const Container &container) noexcept_ndebug
{
    const auto begin = std::begin(container);
    const auto end = std::end(container);

    *this << static_cast<Payload>(std::distance(begin, end));
    return insert(begin, end);
}

template<typename Type, EnableIfContainerNotDetected<Type>*>
inline WritablePacket &WritablePacket::operator<<(const Type &value) noexcept_ndebug
{
    coreAssert(bytesAvailable() >= sizeof(Type),
        throw std::runtime_error("Protocol::WritablePacket::operator<<: Write overflow"));
    header()->payload += sizeof(Type);
    new (currentDataHead()) Type(value);
    _writeIndex += sizeof(Type);
    return *this;
}
