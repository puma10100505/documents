#pragma once

#include "RawBuffer.h"
#include "BufferReader.h"
#include "BufferWriter.h"

namespace yinpsoft
{

static const int DEFAULT_MSS = 540;
static const int MAX_RAW_PACKAGE_SIZE = 500;

#pragma pack(1)
typedef struct stNetMessageHeader
{
    uint8_t cmdid;
    /* REF: https://gafferongames.com/post/virtual_connection_over_udp/ */
    uint32_t appid;

    /* REF: https://gafferongames.com/post/reliability_ordering_and_congestion_avoidance_over_udp/ */
    uint32_t sequence; /// Sequence Numbers
    uint32_t ack;      /// Acks
    uint32_t ack_bitfield;
    int64_t send_time_ms;
    /*---------------------------------------------------------------------------------------------*/
    uint32_t payload_size; /// Content size

    size_t ByteSize() { return sizeof(stNetMessageHeader); }

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt8(cmdid);
        writer.WriteUInt32(appid);
        writer.WriteUInt32(sequence);
        writer.WriteUInt32(ack);
        writer.WriteUInt32(ack_bitfield);
        writer.WriteInt64(send_time_ms);
        writer.WriteUInt32(payload_size);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        cmdid = reader.ReadUInt8();
        appid = reader.ReadUInt32();
        sequence = reader.ReadUInt32();
        ack = reader.ReadUInt32();
        ack_bitfield = reader.ReadUInt32();
        send_time_ms = reader.ReadInt64();
        payload_size = reader.ReadUInt32();
    }

} NetMessageHeader;

typedef struct stRawPackage
{
    uint32_t fragment_idx;
    uint32_t fragment_count;
    size_t pkg_len;
    uint8_t pkg_buff[MAX_RAW_PACKAGE_SIZE];

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt32(fragment_idx);
        writer.WriteUInt32(fragment_count);
        writer.WriteUInt32(pkg_len);
        writer.WriteByteArray(pkg_buff, pkg_len);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        fragment_idx = reader.ReadUInt32();
        fragment_count = reader.ReadUInt32();
        pkg_len = reader.ReadUInt32();
        reader.ReadCharArray(pkg_buff, pkg_len);
    }
} RawPackage;

#pragma pack()

// [Obsolete]
typedef struct stNetMsg
{
    char payload[MAX_PACKET_SIZE];
} NetMsg;

}; // namespace yinpsoft