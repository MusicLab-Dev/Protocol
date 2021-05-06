/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet
 */

#pragma once

#include <cstring>
#include <iterator>
#include <limits>

#include <Core/Assert.hpp>

#include "Protocol.hpp"

namespace Protocol
{
    class ReadablePacket;
    class WritablePacket;

    namespace Internal
    {
        class PacketBase;

        /** @brief Check if iterator type is eligible to a fast range copy */
        template<typename Iterator>
        constexpr bool IsTriviallyRangeCopyable =
            std::is_trivially_copyable_v<decltype(*std::declval<Iterator>())> &&
            std::is_same_v<
                typename std::iterator_traits<Iterator>::iterator_category,
                std::random_access_iterator_tag
            >;
    }

    /** @brief Helper that detects if a type is a container */
    template<typename Type>
    using ContainerDetector = decltype(
        std::begin(std::declval<Type>()),
        std::end(std::declval<Type>())
    );

    /** @brief Helper to enable a function if parameter is a container or not */
    template<typename Type>
    using EnableIfContainerDetected = std::enable_if_t<Core::Utils::IsDetected<ContainerDetector, Type>>;
    template<typename Type>
    using EnableIfContainerNotDetected = std::enable_if_t<!Core::Utils::IsDetected<ContainerDetector, Type>>;
}

/** @brief Base of any packet */
class alignas_eighth_cacheline Protocol::Internal::PacketBase
{
public:
    /** @brief Payload maximum value */
    static constexpr Payload PayloadMax = std::numeric_limits<Payload>::max();

    /** @brief Header of a packet */
    struct alignas(4) Header
    {
        MagicKey magicKey { SpecialLabMagicKey };
        ProtocolType protocolType { ProtocolType::Connection };
        Command command { 0u };
        Payload payload { 0u };
        std::uint16_t footprintStackSize { 0u };
    };

    static_assert_sizeof(Header, 12);

    /** @brief Construct a packet from binary data */
    template<typename BinaryData>
    PacketBase(const BinaryData * const begin, const BinaryData * const end) noexcept_ndebug;

    /** @brief Move constructor */
    PacketBase(PacketBase &&other) noexcept = default;

    /** @brief Destructor */
    ~PacketBase(void) noexcept = default;

    /** @brief Move assignment */
    PacketBase &operator=(PacketBase &&other) noexcept = default;


    /** @brief Get the header's magic key */
    [[nodiscard]] MagicKey magicKey(void) const noexcept { return _header->magicKey; }

    /** @brief Get the packet payload (data size without header) */
    [[nodiscard]] Payload payload(void) const noexcept { return _header->payload; }

    /** @brief Get the total packet size (header and data) */
    [[nodiscard]] Payload totalSize(void) const noexcept { return static_cast<Payload>(payload() + sizeof(Header)); }


    /** @brief Get the protocol type (Connection / Event) */
    [[nodiscard]] ProtocolType protocolType(void) const noexcept { return _header->protocolType; }

    /** @brief Get the packet opaque command */
    [[nodiscard]] Command command(void) const noexcept { return _header->command; }

    /** @brief Get the command casted to a specific type */
    template<typename CommandType>
    [[nodiscard]] CommandType commandAs(void) const noexcept { return static_cast<CommandType>(command()); }


    /** @brief Get the packet footprint stack size */
    [[nodiscard]] std::uint16_t footprintStackSize(void) const noexcept { return _header->footprintStackSize; }

    /** @brief Get the begining of footprint stack pointer */
    [[nodiscard]] const BoardID *footprintStackBegin(void) const noexcept
        { return data() + _header->payload - _header->footprintStackSize; }

    /** @brief Get the end of footprint stack pointer */
    [[nodiscard]] const BoardID *footprintStackEnd(void) const noexcept
        { return data() + _header->payload; }

    /** @brief Get the data pointer */
    template<typename Type = std::uint8_t>
    [[nodiscard]] const Type *data(void) const noexcept { return reinterpret_cast<const Type *>(_header + 1); }

protected:
    /** @brief Get the header pointer */
    [[nodiscard]] const Header *header(void) const noexcept { return _header; }

private:
    const Header *_header { nullptr };
};

static_assert_fit_eighth_cacheline(Protocol::Internal::PacketBase);

/** @brief A read-only packet */
class alignas_quarter_cacheline Protocol::ReadablePacket : public Internal::PacketBase
{
public:
    /** @brief Construct a packet from binary data */
    template<typename BinaryData>
    ReadablePacket(const BinaryData * const begin, const BinaryData * const end) noexcept_ndebug
        : PacketBase(begin, end), _payload(Internal::PacketBase::payload()) {}

    /** @brief Move constructor */
    ReadablePacket(ReadablePacket &&other) noexcept = default;

    /** @brief Destructor */
    ~ReadablePacket(void) noexcept = default;


    /** @brief Move assignment */
    ReadablePacket &operator=(ReadablePacket &&other) noexcept = default;

    /** @brief Extract a value from the packet */
    template<typename Type>
    [[nodiscard]] Type extract(void);

    /** @brief Extract a range of values from the packet */
    template<typename OutputIterator>
    void extract(const OutputIterator begin, const OutputIterator end);

    /** @brief Extract the deserializable data of a container to the packet (including its size) */
    template<typename Container, EnableIfContainerDetected<Container>* = nullptr>
    ReadablePacket &operator>>(Container &container);

    /** @brief Extract deserializable data to the packet */
    template<typename Type, EnableIfContainerNotDetected<Type>* = nullptr>
    ReadablePacket &operator>>(Type &value);

    /** @brief Get the packet payload (data size without header) */
    [[nodiscard]] Payload payload(void) const noexcept { return _payload; }

    /** @brief Get the total packet size (header and data) */
    [[nodiscard]] Payload totalSize(void) const noexcept { return static_cast<Payload>(payload() + sizeof(Header)); }

    /** @brief Returns the remaining writable size available in bytes */
    [[nodiscard]] Payload bytesAvailable(void) const noexcept
        { return static_cast<std::uint16_t>(_payload - _readIndex - header()->footprintStackSize); }

private:
    Payload _payload { 0u };
    Payload _readIndex { 0u };

    // Don't use the base payload / totalSize as we cache it
    using Internal::PacketBase::payload;
    using Internal::PacketBase::totalSize;

    /** @brief Get the data pointer */
    template<typename Type = std::uint8_t>
    [[nodiscard]] const Type *currentDataHead(void) const noexcept
        { return reinterpret_cast<const Type *>(data<std::uint8_t>() + _readIndex); }
};

static_assert_fit_quarter_cacheline(Protocol::ReadablePacket);

/** @brief A write-only packet */
class alignas_quarter_cacheline Protocol::WritablePacket : public Internal::PacketBase
{
public:
    /** @brief Construct a packet from binary data */
    template<typename BinaryData>
    WritablePacket(const BinaryData * const begin, const BinaryData * const end) noexcept_ndebug
        : PacketBase(begin, end), _capacity(static_cast<Payload>(std::distance(begin, end))) {}

    /** @brief Move constructor */
    WritablePacket(WritablePacket &&other) noexcept = default;

    /** @brief Destructor */
    ~WritablePacket(void) noexcept = default;


    /** @brief Move assignment */
    WritablePacket &operator=(WritablePacket &&other) noexcept = default;

    /** @brief Copy assignment from a ReadablePacket */
    WritablePacket &operator=(ReadablePacket &other) noexcept;

    /** @brief Prepare a packet */
    template<typename CommandType, std::enable_if_t<sizeof(CommandType) == sizeof(Command)>* = nullptr>
    WritablePacket &prepare(const ProtocolType protocolType, const CommandType command);

    /** @brief Insert a range of values in the packet */
    template<typename InputIterator>
    WritablePacket &insert(const InputIterator begin, const InputIterator end) noexcept_ndebug;

    /** @brief Insert the serializable data of a container to the packet */
    template<typename Container, EnableIfContainerDetected<Container>* = nullptr>
    WritablePacket &operator<<(const Container &container) noexcept_ndebug;

    /** @brief Insert trivially serializable data to the packet */
    template<typename Type, EnableIfContainerNotDetected<Type>* = nullptr>
    WritablePacket &operator<<(const Type &value) noexcept_ndebug;

    /** @brief Returns the remaining writable size available in bytes */
    [[nodiscard]] Payload bytesAvailable(void) const noexcept
        { return static_cast<Payload>(_capacity - _writeIndex - static_cast<Payload>(sizeof(Header))); }

    /** @brief push a boardID at the end of the footprint stack */
    void pushFootprint(const BoardID boardID);

    /** @brief remove the boardID at the front of the footprint stack and return the value */
    BoardID popFrontStack(void);

    /** @brief remove the boardID at the end of the footprint stack and return the value */
    BoardID popBackStack(void);

    /** @brief Get the data pointer */
    template<typename Type = std::uint8_t>
    [[nodiscard]] Type *data(void) noexcept
        { return const_cast<Type *>(Internal::PacketBase::data<Type>()); }

private:
    Payload _capacity { 0u };
    Payload _writeIndex { 0u };

    /** @brief Get the header pointer */
    [[nodiscard]] Header *header(void) const noexcept
        { return const_cast<Header *>(Internal::PacketBase::header()); }


    /** @brief Get the data pointer */
    template<typename Type = std::uint8_t>
    [[nodiscard]] Type *currentDataHead(void) noexcept
        { return reinterpret_cast<Type *>(data<std::uint8_t>() + _writeIndex); }
};

static_assert_fit_quarter_cacheline(Protocol::WritablePacket);

#include "Packet.ipp"
