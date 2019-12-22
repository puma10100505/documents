#pragma once

#include "NetSocket.h"
namespace yinpsoft
{
// TODO: store client list info and calc timeout
class RUDPServer
{
public:
    RUDPServer() {}
    ~RUDPServer() {}

    int Initialize(uint32_t appid, unsigned short port);
    void Tick(int fps = 30);
    void Stop();

private:
    void SerializeData(const char *data, size_t len);
    void DumpPacket(const char *packet, size_t plen);

private:
    NetSocket svr_socket;
    int64_t svr_tick;
    uint32_t application_id;
    uint32_t seq;
    uint32_t peer_seq;
    uint32_t pack_size;

    std::chrono::high_resolution_clock::time_point last_client_packet_timestamp;
    char raw_data[MAX_PACKET_SIZE];
    char payload_data[MAX_PACKET_SIZE];
};
}; // namespace yinpsoft