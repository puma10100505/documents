#pragma once

#include "NetSocket.h"
#include "NetAddress.h"
#include "NetMessage.h"
#include "Commands.h"
#include <unordered_map>
#include <chrono>
#include <vector>
#include "PropertyMacros.h"
#include "gameplay.pb.h"

using namespace std;
using namespace yinpsoft;

#define MAX_TRY_TIMES_FOR_SERVER_RESP 10

namespace yinpsoft
{

typedef struct 
{
    uint8_t buff[MAX_RAW_PACKAGE_SIZE];
    size_t len;

} SendingPackage;

typedef struct 
{
    uint8_t cmd;
    union {
        RawPackage raw;
        StartResp start;
        QuitResp quit;    
    } package;
    pb::PBGameObject go;
} ReceivedPackage;

enum EClientStatus {
    CS_NONE = 0,
    CS_START,
    CS_ENTER,
    CS_READY,
    CS_QUIT,
    CS_MAX
};

class RUDPClient 
{
public:
    RUDPClient()
    {
        command_map["quit"] = ENetCommandID::NET_CMD_QUIT;
        command_map["start"] = ENetCommandID::NET_CMD_START;
    }

    ~RUDPClient()
    {
        // shared_memory_object::remove("SHM_COMMAND_LIST");
    }

    virtual void Initialize(uint32_t appid, unsigned int address,
                           unsigned short port, bool shm = false,
                           int32_t interval_ms = 100);

    void Run();
    void Stop();

    inline void PushCommandLine(const ENetCommandID &line) { local_command_list.push_back(line); }
    inline int32_t CommandCount() { return static_cast<int32_t>(local_command_list.size()); }

    void SendThread();
    void RecvThread();
    void UpdateThread();
    

private:
    void DumpPacket(const char *packet, size_t plen);
    void DumpBuffer(RawBuffer &buff);

    void NetCommandDispatcher(const int32_t cmd);
    void SendPackage(BufferWriter &writer);

    void OnInput();
    void OnSend();
    void OnCommandDispatch();
    void OnRecv();
    bool OnValidate(const NetHeader &header);
    virtual void OnUpdate();

private:
    void PerformQuit();
    void PerformHeartbeat();
    void PerformData();
    void PerformStart();
    void PerformPlayerEnter();

    

private:
    uint32_t application_id;
    NetSocket cli_socket;
    NetAddress svr_address;
    uint32_t seq;
    uint32_t remote_seq;
    uint32_t pack_size;
    int32_t interval;
    uint32_t client_tick;
    GETSETVAR(uint32_t, sid, 0);
    GETSETVAR(uint64_t, battle_id, 0);
    GETSETVAR(uint64_t, guid, 0);

    bool enable_shm = false;

    // shm_vector *shm_command_list = nullptr;
    std::string command_params;

    // 客户端发送队列
    std::vector<SendingPackage> pending_send_list;
    std::vector<ReceivedPackage> pending_recv_list;
    std::vector<ENetCommandID> local_command_list;
    std::unordered_map<std::string, ENetCommandID> command_map;

    GETSETVAR(bool, running, false);
    GETSETVAR(EClientStatus, status, EClientStatus::CS_NONE);
};
}; // namespace yinpsoft
