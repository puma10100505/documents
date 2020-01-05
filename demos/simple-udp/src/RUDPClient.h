#pragma once
#include "NetSocket.h"
#include "NetAddress.h"
#include "NetMessage.h"
#include "Commands.h"
#include <unordered_map>
#include <chrono>
#include <vector>

// #include "boost/interprocess/managed_shared_memory.hpp"
// #include "boost/interprocess/containers/vector.hpp"
// #include "boost/interprocess/allocators/allocator.hpp"

using namespace std;
using namespace yinpsoft;
// using namespace boost::interprocess;

#define MAX_TRY_TIMES_FOR_SERVER_RESP 10

// typedef boost::interprocess::allocator<int, managed_shared_memory::segment_manager> ShmAllocator;
// typedef boost::interprocess::vector<int, ShmAllocator> shm_vector;

namespace yinpsoft
{

typedef struct stClientPackage
{
    uint8_t buff[MAX_RAW_PACKAGE_SIZE];
    size_t len;
} ClientPackage;

typedef struct stResponsePackage
{
    uint8_t cmd;
    union {
        RawPackage raw;
        StartResponse start;
    } package;

} ResponsePackage;

class RUDPClient final
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

    RUDPClient &Initialize(uint32_t appid, unsigned int address,
                           unsigned short port, bool shm = false,
                           int32_t interval_ms = 100);

    void Run();
    void Stop();

    inline void PushCommandLine(const ENetCommandID &line) { local_command_list.push_back(line); }
    inline int32_t CommandCount() { return static_cast<int32_t>(local_command_list.size()); }

private:
    void DumpPacket(const char *packet, size_t plen);
    void DumpBuffer(RawBuffer &buff);

    // void AcceptStdInput(std::string &input_str);
    // void NetCommandDispacher(const std::string &command);
    void NetCommandDispatcher(const int32_t cmd);
    void AddClientPackage(BufferWriter &writer);

    void OnInput();
    void OnSend();
    void OnCommandDispatch();
    void OnRecv();
    bool OnValidate(const NetMessageHeader &header);
    void OnUpdate();

private:
    void PerformQuit();
    void PerformHeartbeat();
    void PerformData();
    void PerformStart();

    void ResolveStart(const StartResponse &pkg);

private:
    uint32_t application_id;
    NetSocket cli_socket;
    NetAddress svr_address;
    uint32_t seq;
    uint32_t remote_seq;
    uint32_t pack_size;
    int32_t interval;
    uint32_t client_tick;

    bool enable_shm = false;

    // shm_vector *shm_command_list = nullptr;
    std::string command_params;

    // 客户端发送队列
    std::vector<ClientPackage> pending_send_list;
    std::vector<ResponsePackage> pending_recv_list;
    std::vector<ENetCommandID> local_command_list;
    std::unordered_map<std::string, ENetCommandID> command_map;
};
}; // namespace yinpsoft
