#pragma once

#include "RawBuffer.h"

namespace google
{
namespace protobuf
{
class Message;
}
} // namespace google

namespace yinpsoft
{
class RawBuffer;

class BufferReader
{
public:
    BufferReader() {}
    BufferReader(const uint8_t *data, size_t len);
    BufferReader(const std::string &data);
    ~BufferReader() {}

public:
    inline uint8_t Read() { return raw_buffer.ReadByte(); }
    // inline uint16_t Read() { return ReadUInt16(); }
    // inline uint32_t Read() { return ReadUInt32(); }
    // inline uint64_t Read() { return ReadUInt64(); }
    // inline int16_t Read() { return ReadInt16(); }
    // inline int32_t Read() { return ReadInt32(); }
    // inline int64_t Read() { return ReadInt64(); }

    inline uint16_t ReadUInt16() { return ReadInternal<uint16_t>(); }
    inline uint32_t ReadUInt32() { return ReadInternal<uint32_t>(); }
    inline uint64_t ReadUInt64() { return ReadInternal<uint64_t>(); }

    inline int16_t ReadInt16() { return ReadInternal<int16_t>(); }
    inline int32_t ReadInt32() { return ReadInternal<int32_t>(); }
    inline int64_t ReadInt64() { return ReadInternal<int64_t>(); }

    bool ReadBool() { return static_cast<bool>(raw_buffer.ReadByte()); }
    size_t ReadString(uint8_t *value, size_t len);

    bool ReadProto(google::protobuf::Message &msg);

private:
    template <typename T>
    T ReadInternal()
    {
        T value = 0;

        for (uint32_t i = 0; i < sizeof(T); i++)
        {
            value |= (raw_buffer.ReadByte() << (i * 8));
        }

        return value;
    }

private:
    RawBuffer raw_buffer;
};
}; // namespace yinpsoft