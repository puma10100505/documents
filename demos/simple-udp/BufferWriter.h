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

class BufferWriter
{
public:
    BufferWriter() { raw_buffer.Initialize(); }
    ~BufferWriter() {}

    inline bool IsWriteFull() const { return raw_buffer.IsWriteFull(); }

    inline const RawBuffer &Raw() { return raw_buffer; }
    inline RawBuffer &MutableRaw() { return raw_buffer; }

public:
    inline void WriteUInt8(uint8_t value) { raw_buffer.WriteByte(value); }
    inline void WriteUInt32(uint32_t value) { WriteInternal<uint32_t>(value); }
    inline void WriteUInt64(uint64_t value) { WriteInternal<uint64_t>(value); }
    inline void WriteUInt16(uint16_t value) { WriteInternal<uint16_t>(value); }

    inline void WriteInt32(int32_t value) { WriteInternal<int32_t>(value); }
    inline void WriteInt64(int64_t value) { WriteInternal<int64_t>(value); }
    inline void WriteInt16(int16_t value) { WriteInternal<int16_t>(value); }

    void WriteString(const uint8_t *str, size_t len);
    void WriteBool(bool value);

    bool WriteProto(const google::protobuf::Message &msg);

private:
    template <typename T>
    void WriteInternal(T value)
    {
        for (uint32_t i = 0; i < sizeof(T); i++)
        {
            raw_buffer.WriteByte((value >> i * 8) & 0xff);
        }
    }

private:
    RawBuffer raw_buffer;
};
}; // namespace yinpsoft