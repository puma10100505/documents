#pragma once

#include "RawBuffer.h"
#include "BufferReader.h"
#include "BufferWriter.h"

namespace yinpsoft
{

#pragma pack(1)
typedef struct stNetMessageHeader
{
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
        appid = reader.ReadUInt32();
        sequence = reader.ReadUInt32();
        ack = reader.ReadUInt32();
        ack_bitfield = reader.ReadUInt32();
        send_time_ms = reader.ReadInt64();
        payload_size = reader.ReadUInt32();
    }

} NetMessageHeader;
#pragma pack()

// [Obsolete]
typedef struct stNetMsg
{
    char payload[MAX_PACKET_SIZE];
} NetMsg;

}; // namespace yinpsoft