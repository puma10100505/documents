#pragma once
#include "NetSocket.h"
#include "NetAddress.h"

using namespace yinpsoft;

namespace yinpsoft
{
class RUDPClient final
{
public:
    RUDPClient() {}
    ~RUDPClient() {}

    int Initialize(uint32_t appid, unsigned int address,
                   unsigned short port, int32_t timeout = 500);

    void Run();
    void Stop();

private:
    void DumpPacket(const char *packet, size_t plen);
    void SerializeData(const char *payload, size_t len);

private:
    uint32_t application_id;
    NetSocket cli_socket;
    NetAddress svr_address;
    int32_t wait_server_timeout;
    uint32_t seq;
    uint32_t peer_seq;
    uint32_t pack_size;

    char request_packet[MAX_PACKET_SIZE];
    char response_packet[MAX_PACKET_SIZE];
};
}; // namespace yinpsoft