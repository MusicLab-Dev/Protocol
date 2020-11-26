/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet
 */

#include <stdexcept>


template<typename BinaryData>
inline Protocol::Internal::PacketBase::PacketBase(const BinaryData * const begin, [[maybe_unused]] const BinaryData * const end) noexcept_ndebug
    : _header(reinterpret_cast<const Header * const>(begin))
{
    coreAssert(static_cast<std::size_t>(std::distance(begin, end)) >= sizeof(Header),
        throw std::logic_error("Protocol::PacketBase::PacketBase: Invalid packet data size must be equal or greater than header size"));
}


template<typename Type>
Type Protocol::ReadablePacket::extract(void) noexcept_ndebug
{
    Type value;

    *this >> value;
    return value;
}

template<typename OutputIterator>
void Protocol::ReadablePacket::extract(const OutputIterator begin, const OutputIterator end) noexcept_ndebug
{
    using Type = decltype(*std::declval<OutputIterator>());

    constexpr bool IsTriviallyRangeCopyable =
        std::is_trivially_copyable_v<Type> &&
        std::is_same_v<
            typename std::iterator_traits<OutputIterator>::iterator_category,
            std::random_access_iterator_tag
        >;

    const auto size = std::distance(begin, end);
    const auto sizeInBytes = size * sizeof(Type);

    // Check if the container contains optimizable trivially copyable types
    if constexpr (IsTriviallyRangeCopyable) {
        coreAssert(bytesAvailable() >= size * sizeof(Type),
            throw std::runtime_error("Protocol::WritablePacket::insert: Write overflow"));
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
Protocol::ReadablePacket &Protocol::ReadablePacket::operator>>(Container &container) noexcept_ndebug
{
    container.resize(extract<std::uint16_t>());
    extract(std::begin(container), std::end(container));
    return *this;
}

template<typename Type, Protocol::EnableIfContainerNotDetected<Type>*>
Protocol::ReadablePacket &Protocol::ReadablePacket::operator>>(Type &value) noexcept_ndebug
{
    coreAssert(bytesAvailable() >= sizeof(Type),
        throw std::runtime_error("Protocol::ReadablePacket::operator>>: Read overflow"));
    new (&value) Type { *currentDataHead<Type>() };
    _readIndex += sizeof(Type);
    return *this;
}


template<typename CommandType, std::enable_if_t<sizeof(CommandType) == sizeof(Protocol::Command)>*>
inline Protocol::WritablePacket &Protocol::WritablePacket::prepare(const ProtocolType protocolType, const CommandType command)
{
    _writeIndex = 0u;
    header()->magicKey = SpecialLabMagicKey;
    header()->protocolType = protocolType;
    header()->command = static_cast<Command>(command);
    header()->payload = 0u;
    header()->footprintStack = 0u;
    return *this;
}

template<typename InputIterator>
inline Protocol::WritablePacket &Protocol::WritablePacket::insert(const InputIterator begin, const InputIterator end) noexcept_ndebug
{
    using Type = decltype(*std::declval<InputIterator>());

    constexpr bool IsTriviallyRangeCopyable =
        std::is_trivially_copyable_v<Type> &&
        std::is_same_v<
            typename std::iterator_traits<InputIterator>::iterator_category,
            std::random_access_iterator_tag
        >;

    const auto size = std::distance(begin, end);
    const auto sizeInBytes = size * sizeof(Type);

    // Check if the container contains optimizable trivially copyable types
    if constexpr (IsTriviallyRangeCopyable) {
        coreAssert(bytesAvailable() >= size * sizeof(Type),
            throw std::runtime_error("Protocol::WritablePacket::insert: Write overflow"));
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

template<typename Container, Protocol::EnableIfContainerDetected<Container>*>
Protocol::WritablePacket &Protocol::WritablePacket::operator<<(const Container &container) noexcept_ndebug
{
    const auto begin = std::begin(container);
    const auto end = std::end(container);

    *this << static_cast<std::uint16_t>(std::distance(begin, end));
    return insert(begin, end);
}

template<typename Type, Protocol::EnableIfContainerNotDetected<Type>*>
inline Protocol::WritablePacket &Protocol::WritablePacket::operator<<(const Type &value) noexcept_ndebug
{
    coreAssert(bytesAvailable() >= sizeof(Type),
        throw std::runtime_error("Protocol::WritablePacket::operator<<: Write overflow"));
    header()->payload += sizeof(Type);
    new (currentDataHead()) Type(value);
    _writeIndex += sizeof(Type);
    return *this;
}