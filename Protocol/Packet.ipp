/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Packet
 */

template<typename CommandType, std::enable_if_t<sizeof(CommandType) == sizeof(Protocol::Command)>*>
Protocol::WritablePacket &Protocol::WritablePacket::prepare(const ProtocolType protocolType, const CommandType command)
{
    _header->magicKey = MagicKey;
    _header->protocolType = protocolType;
    _header->command = static_cast<Command>(command);
    _header->payload = 0u;
    return *this;
}

template<typename Type>
Protocol::WritablePacket &Protocol::WritablePacket::operator<<(Type &&other) noexcept_ndebug
{

}