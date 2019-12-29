#pragma once
#include "NetSocket.h"
#include "NetAddress.h"
#include "NetMessage.h"

using namespace yinpsoft;

#define MAX_TRY_TIMES_FOR_SERVER_RESP 10

namespace yinpsoft
{
class RUDPClient final
{
public:
    RUDPClient() {}
    ~RUDPClient() {}

    RUDPClient &Initialize(uint32_t appid, unsigned int address,
                           unsigned short port, int32_t timeout_sec = 5);

    void Run();
    void Stop();

private:
    void DumpPacket(const char *packet, size_t plen);
    void SerializeData(const char *payload, size_t len, BufferWriter& writer);
    void DumpBuffer(RawBuffer& buff);

private:
    uint32_t application_id;
    NetSocket cli_socket;
    NetAddress svr_address;
    int32_t wait_server_timeout;
    uint32_t seq;
    uint32_t remote_seq;
    uint32_t pack_size;

    char request_packet[MAX_RAW_PACKAGE_SIZE];
    char response_packet[MAX_RAW_PACKAGE_SIZE];
};
}; // namespace yinpsoft