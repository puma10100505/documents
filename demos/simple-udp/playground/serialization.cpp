#include <stdio.h>
#include <string.h>
#include <string>
#include <stddef.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <byteswap.h>
#include "net_stream.h"

#define MAX_BUFFER_SIZE 100 * 1024

using namespace std;

class PacketBuffer
{
public:
    PacketBuffer()
    {
        size = MAX_BUFFER_SIZE;
        index = 0;
        data = (uint8_t *)malloc(size);
    }
    PacketBuffer(const std::string &data);
    PacketBuffer(const char *data, size_t len);
    ~PacketBuffer()
    {
        /* TODO: */
        free(data);
        index = 0;
    }

    size_t Size() { return size; }
    size_t Position() { return index; }
    void Rewind() { index = 0; }

public:
    // Witers
    void WriteUInt32(uint32_t value);
    void WriteUInt16(uint16_t value);
    void WriteUInt64(uint64_t value);

    void WriteByte(uint8_t value);
    void WriteBool(bool value);

    void WriteInt64(int64_t value);
    void WriteInt32(int32_t value);
    void WriteInt16(int16_t value);

    // Readers
    uint32_t ReadUInt32();
    uint16_t ReadUInt16();
    uint64_t ReadUInt64();

    uint8_t ReadByte();
    bool ReadBool();

    int32_t ReadInt32();
    int16_t ReadInt16();
    int64_t ReadInt64();

private:
    uint8_t *data;
    size_t size;
    size_t index;
};

PacketBuffer::PacketBuffer(const std::string &str)
{
    if (index + str.length() < size)
    {
        memcpy(data + index, (const void *)str.c_str(), str.length());
        index += str.length();
    }
}

PacketBuffer::PacketBuffer(const char *indata, size_t len)
{
    if (index + len < size)
    {
        memcpy(data + index, (const void *)indata, len);
        index += len;
    }
}

void PacketBuffer::WriteUInt32(uint32_t value)
{
    assert(index + sizeof(value) <= size);
#ifdef BIG_ENDIAN
    *((uint32_t *)(data + index)) = bswap_32(value);
#else
    *((uint32_t *)(data + index)) = value;
#endif
    index += sizeof(value);
}

uint32_t PacketBuffer::ReadUInt32()
{
    assert(index + sizeof(uint32_t) <= size);

    uint32_t value;

#ifdef BIG_ENDIAN
    value = bswap_32(*((uint32_t *)(data + index)));
#else
    value = *((uint32_t *)(data + index));
#endif
    index += sizeof(uint32_t);
    return value;
}

void PacketBuffer::WriteByte(uint8_t value)
{
    assert(index + sizeof(uint8_t) <= size);

    *((uint8_t *)(data + index)) = value;
    index += sizeof(value);
}

uint8_t PacketBuffer::ReadByte()
{
    assert(index + sizeof(uint8_t) <= size);

    uint8_t value;

    value = *((uint8_t *)(data + index));
    index += sizeof(uint8_t);

    return value;
}

void PacketBuffer::WriteBool(bool value)
{
    assert(index + sizeof(bool) <= size);

    *((bool *)(data + index)) = value;
    index += sizeof(bool);
}

bool PacketBuffer::ReadBool()
{
    assert(index + sizeof(bool) <= size);

    bool value;
    value = *((uint8_t *)(data + index));
    index += sizeof(uint8_t);

    return value;
}

void PacketBuffer::WriteInt32(int32_t value)
{
    assert(index + sizeof(int32_t) <= size);

#ifdef BIG_ENDIAN
    *((int32_t *)(data + index)) = bswap_32(value);
#else
    *((int32_t *)(data + index)) = value;
#endif

    index += sizeof(int32_t);
}

int32_t PacketBuffer::ReadInt32()
{
    assert(index + sizeof(int32_t) <= size);

    int32_t value;

#ifdef BIG_ENDIAN
    value = bswap_32(*((int32_t *)(data + index)));
#else
    value = *((int32_t *)(data + index));
#endif
    index += sizeof(int32_t);

    return value;
}

void PacketBuffer::WriteInt16(int16_t value)
{
    assert(index + sizeof(int16_t) <= size);

#ifdef BIG_ENDIAN
    *((int16_t *)(data + index)) = bswap_16(value);
#else
    *((int16_t *)(data + index)) = value;
#endif

    index += sizeof(int16_t);
}

int16_t PacketBuffer::ReadInt16()
{
    assert(index + sizeof(int16_t) <= size);

    int16_t value;

#ifdef BIG_ENDIAN
    value = bswap_16(*((int16_t *)(data + index)));
#else
    value = *((int16_t *)(data + index));
#endif
    index += sizeof(int16_t);

    return value;
}

void PacketBuffer::WriteInt64(int64_t value)
{
    assert(index + sizeof(int64_t) <= size);

#ifdef BIG_ENDIAN
    *((int64_t *)(data + index)) = bswap_64(value);
#else
    *((int64_t *)(data + index)) = value;
#endif

    index += sizeof(int64_t);
}

int64_t PacketBuffer::ReadInt64()
{
    assert(index + sizeof(int64_t) <= size);

    int64_t value;

#ifdef BIG_ENDIAN
    value = bswap_64(*((int64_t *)(data + index)));
#else
    value = *((int64_t *)(data + index));
#endif
    index += sizeof(int64_t);

    return value;
}

void PacketBuffer::WriteUInt64(uint64_t value)
{
    assert(index + sizeof(uint64_t) <= size);

#ifdef BIG_ENDIAN
    *((uint64_t *)(data + index)) = bswap_64(value);
#else
    *((uint64_t *)(data + index)) = value;
#endif

    index += sizeof(uint64_t);
}

uint64_t PacketBuffer::ReadUInt64()
{
    assert(index + sizeof(uint64_t) <= size);

    uint64_t value;

#ifdef BIG_ENDIAN
    value = bswap_64(*((uint64_t *)(data + index)));
#else
    value = *((uint64_t *)(data + index));
#endif
    index += sizeof(uint64_t);

    return value;
}

void PacketBuffer::WriteUInt16(uint16_t value)
{
    assert(index + sizeof(uint16_t) <= size);

#ifdef BIG_ENDIAN
    *((uint16_t *)(data + index)) = bswap_64(value);
#else
    *((uint16_t *)(data + index)) = value;
#endif

    index += sizeof(uint16_t);
}

uint16_t PacketBuffer::ReadUInt16()
{
    assert(index + sizeof(uint16_t) <= size);

    uint16_t value;

#ifdef BIG_ENDIAN
    value = bswap_16(*((uint16_t *)(data + index)));
#else
    value = *((uint16_t *)(data + index));
#endif
    index += sizeof(uint16_t);

    return value;
}

typedef struct PacketA
{
    int x, y, z;
} Packet;

int main()
{
    Packet p;
    PacketBuffer pb;

    p.x = 10;
    p.y = 22;
    p.z = 89;

    pb.WriteUInt32(p.x);
    pb.WriteUInt32(p.y);
    pb.WriteUInt32(p.z);

    std::cout << pb.Size() << std::endl;
    std::cout << pb.Position() << std::endl;

    pb.Rewind();

    int32_t x = pb.ReadUInt32();
    int32_t y = pb.ReadUInt32();
    int32_t z = pb.ReadUInt32();

    std::cout << "after read: " << std::endl;

    std::cout << x << " " << y << " " << z << std::endl;

    int bits_required = net::Stream::BitsRequired(0, 1);

    printf("%x\n", ~1);
    std::cout << bits_required << std::endl;

    printf("%x, %d\n", (1 << 8) - 1, 198 & (1 << 8) - 1);

    return 0;
}