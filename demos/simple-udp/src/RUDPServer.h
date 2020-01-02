#pragma once

#include "NetSocket.h"
#include "NetMessage.h"

#include <vector>

using namespace std;

namespace yinpsoft
{
// TODO: store client list info and calc timeout

class RUDPServer final
{
public:
    RUDPServer() {}
    ~RUDPServer() {}

    RUDPServer &Initialize(uint32_t appid, unsigned short port);
    void Tick();
    void Stop();

private:
    // [Obsolete]
    void SerializeData(const char *data, size_t len);
    void DumpPacket(const char *packet, size_t plen);

    // [Obsolete]
    void RecvBytesFromNetwork();
    bool OnValidate(const NetMessageHeader& header);
    void CommandDispatcher(uint8_t cmd, const RawPackage& pkg);
    void OnRecvBytes();

private:
    void HandleQuitMessage(const RawPackage& pkg);
    void HandleHeartbeatMessage(const RawPackage& pkg);
    void HandleDataMessage(const RawPackage& pkg);
    void HandleStartMessage(const RawPackage& pkg);

private:
    NetSocket svr_socket;
    int64_t svr_tick;
    uint32_t application_id;
    uint32_t seq;
    uint32_t remote_seq;
    uint32_t pack_size;

    std::vector<RawPackage> recv_queue; /// 接收队列
    std::vector<RawPackage> send_queue; /// 发送队列

    std::chrono::high_resolution_clock::time_point last_client_packet_timestamp;
    char raw_data[MAX_PACKET_SIZE];
    char payload_data[MAX_PACKET_SIZE];
};
}; // namespace yinpsoft