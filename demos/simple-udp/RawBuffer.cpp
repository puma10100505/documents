#include "RawBuffer.h"

using namespace yinpsoft;

void RawBuffer::Initialize(const std::string &str)
{
    capacity = MAX_RAW_BUFFER_SIZE;

    if (index + str.length() < capacity)
    {
        memcpy(buffer + index, (const void *)str.c_str(), str.length());
        index += str.length();
        length = index;
    }
}

void RawBuffer::Initialize(const uint8_t *data, size_t len)
{
    capacity = MAX_RAW_BUFFER_SIZE;

    if (index + len < capacity)
    {
        memcpy(buffer + index, (const void *)data, len);
        index += len;
        length = index;
    }
}

void RawBuffer::WriteByte(uint8_t value)
{
    assert(index + sizeof(uint8_t) <= capacity);

    *((uint8_t *)(buffer + index)) = value;
    index += sizeof(uint8_t);
    length = index;
}

uint8_t RawBuffer::ReadByte()
{
    assert(IsEmpty() == false);                // buffer cannot be empty
    assert(index + sizeof(uint8_t) <= length); // can not read more than length

    uint8_t value;

    value = *((uint8_t *)(buffer + index));
    index += sizeof(uint8_t);

    return value;
}

void RawBuffer::WriteBytes(const uint8_t *value, size_t len)
{
    assert(index + len <= capacity);

    memcpy(buffer + index, value, len);
    index += len;
    length = index;
}

size_t RawBuffer::ReadBytes(uint8_t *value, size_t len)
{
    assert(IsEmpty() == false);
    assert(index + len <= length);

    memcpy(value, buffer + index, len);
    return len;
}