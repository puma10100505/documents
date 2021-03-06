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
    uint32_t sid;

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
        writer.WriteUInt32(sid);

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
        sid = reader.ReadUInt32();
    }

    void PrintString()
    {
        printf("cmdid: %d, appid: %u, seq: %u, ack: %u, ack_bitfield: %u, send_time_ms: %ld, payload_size: %u, sid: %u\n",
               cmdid, appid, sequence, ack, ack_bitfield, send_time_ms, payload_size, sid);
    }

} NetMessageHeader;

typedef struct stRawPackage
{
    uint64_t guid;
    uint32_t fragment_idx;
    uint32_t fragment_count;
    size_t pkg_len;
    uint8_t pkg_buff[MAX_RAW_PACKAGE_SIZE];

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(guid);
        writer.WriteUInt32(fragment_idx);
        writer.WriteUInt32(fragment_count);
        writer.WriteUInt32(pkg_len);
        writer.WriteByteArray(pkg_buff, pkg_len);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        guid = reader.ReadUInt64();
        fragment_idx = reader.ReadUInt32();
        fragment_count = reader.ReadUInt32();
        pkg_len = reader.ReadUInt32();
        reader.ReadCharArray(pkg_buff, pkg_len);
    }

    void PrintString()
    {
        printf("fragment_idx: %u, fragmanet_count: %u, pkg_len: %lu, pkg_buff: %s\n",
               fragment_idx, fragment_count, pkg_len, pkg_buff);
    }
} RawPackage;

typedef struct
{
    uint8_t cmd;
    uint32_t sid;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt8(cmd);
        writer.WriteUInt32(sid);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        cmd = reader.ReadUInt8();
        sid = reader.ReadUInt32();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 256, "cmd: %u|sid: %u", cmd, sid);
        return str;
    }
} NetHeader;

typedef struct
{
    uint64_t guid;
    uint32_t udid;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(guid);
        writer.WriteUInt32(udid);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        guid = reader.ReadUInt64();
        udid = reader.ReadUInt32();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 128, "guid: %lu|udid: %u", guid, udid);
        return str;
    }

} StartReq;

// Start命令的回包协议
typedef struct 
{
    uint64_t guid;
    uint32_t sid;
    uint64_t battle_id;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(guid);
        writer.WriteUInt32(sid);
        writer.WriteUInt64(battle_id);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        guid = reader.ReadUInt64();
        sid = reader.ReadUInt32();
        battle_id = reader.ReadUInt64();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 128, "guid: %lu|sid: %u|battle_id: %lu", guid, sid, battle_id);
        return str;
    }

} StartResp;

typedef struct
{
    uint64_t guid;
    uint32_t sid;
    uint64_t battle_id;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(guid);
        writer.WriteUInt32(sid);
        writer.WriteUInt64(battle_id);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        guid = reader.ReadUInt64();
        sid = reader.ReadUInt32();
        battle_id = reader.ReadUInt64();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 128, "guid: %lu|sid: %u|battle_id: %lu", guid, sid, battle_id);
        return str;
    }
} QuitReq;

typedef struct stQuitResp
{
    uint64_t guid;
    uint32_t sid;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(guid);
        writer.WriteUInt32(sid);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        guid = reader.ReadUInt64();
        sid = reader.ReadUInt32();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 128, "guid: %lu|sid: %u", guid, sid);
        return str;
    }
} QuitResp;

typedef struct 
{
    uint64_t guid;
    uint32_t sid;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(guid);
        writer.WriteUInt32(sid);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        guid = reader.ReadUInt64();
        sid = reader.ReadUInt32();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 128, "guid: %lu|sid: %u", guid, sid);
        return str;
    }
} OpenWorldReq;

typedef struct 
{
    uint64_t battle_id;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(battle_id);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        battle_id = reader.ReadUInt64();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 128, "battle_id: %lu", battle_id);
        return str;
    }
} OpenWorldResp;

typedef struct 
{
    uint64_t guid;
    uint32_t sid;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(guid);
        writer.WriteUInt32(sid);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        guid = reader.ReadUInt64();
        sid = reader.ReadUInt32();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 128, "guid: %lu|sid: %u", guid, sid);
        return str;
    }
} EnterReq;

typedef struct
{
    uint64_t battle_id;

    size_t Serialize(yinpsoft::BufferWriter &writer)
    {
        writer.WriteUInt64(battle_id);

        return writer.Raw().Length();
    }

    void Deserialize(yinpsoft::BufferReader &reader)
    {
        battle_id = reader.ReadUInt64();
    }

    std::string ToString() const
    {
        char str[128];
        snprintf(str, 128, "battle_id: %lu", battle_id);
        return str;
    }
} EnterResp;

#pragma pack()

}; // namespace yinpsoft