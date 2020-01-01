#include "Header.h"

#include "NetSocket.h"
#include "Singleton.h"
#include "RUDPClient.h"
#include "RUDPServer.h"
#include "BufferWriter.h"
#include "BufferReader.h"
#include "Commands.h"
#include "GameEngine.h"
#include <chrono>

#include "proto/src/NetMessage.h"
#include "google/protobuf/message.h"
#include "boost/thread/thread.hpp"
#include "boost/interprocess/managed_shared_memory.hpp"
#include "boost/interprocess/containers/vector.hpp"
#include "boost/interprocess/allocators/allocator.hpp"

using namespace yinpsoft;
using namespace boost::interprocess;

static shm_vector *shm_command_list = nullptr;

static void DestroyShmVector()
{
    shared_memory_object::remove("SHM_COMMAND_LIST");
}

static void InitShmVector()
{
    DestroyShmVector();

    managed_shared_memory segment(open_or_create, "SHM_COMMAND_LIST", 65536);
    const ShmAllocator alloc_inst(segment.get_segment_manager());
    shm_command_list = segment.find_or_construct<shm_vector>("COMMAND_LIST")(alloc_inst);
}

static void AcceptStdInput()
{
    while (true)
    {
        std::string input_str;
        getline(std::cin, input_str);
        std::transform(input_str.begin(), input_str.end(), input_str.begin(), ::tolower);

        if (shm_command_list != nullptr)
        {
            if ("quit" == input_str)
            {
                shm_command_list->push_back(ENetCommandID::NET_CMD_QUIT);
            }

            printf("after push command: %s, command count in shm: %lu\n", input_str.c_str(), shm_command_list->size());
        }
        else
        {
            printf("command list in shm is null\n");
            return;
        }
    }
}

int Codecode()
{
    yinpsoft::BufferWriter bw;
    NetMessageHeader header;
    header.ack = 1;
    header.ack_bitfield = 10;
    header.appid = APPID;
    header.payload_size = sizeof("hello world");
    header.send_time_ms = time(0);
    header.sequence = 2;

    printf("BEFORE: appid: %u, ack: %u, ack_bitfield: %u, payload_size: %u, time: %ld, seq: %u\n",
           header.appid, header.ack, header.ack_bitfield, header.payload_size, header.send_time_ms, header.sequence);

    size_t len = header.Serialize(bw);

    std::cout << "after serialize: " << len << std::endl;

    NetMessageHeader header2;

    yinpsoft::BufferReader br(bw.Raw().Buffer(), bw.Raw().Length());
    br.MutableRaw().Rewind();
    header2.Deserialize(br);

    printf("AFTER: appid: %u, ack: %u, ack_bitfield: %u, payload_size: %u, time: %ld, seq: %u\n",
           header2.appid, header2.ack, header2.ack_bitfield, header2.payload_size, header2.send_time_ms, header2.sequence);

    return 0;
}

void InitCommandMap()
{
    COMMAND_MAP["quit"] = ENetCommandID::NET_CMD_QUIT;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("the paramter num is illegal\n");
        return -1;
    }

    InitCommandMap();

    char *type = argv[1];

    if (strncmp(type, "server", sizeof("server")) == 0)
    {
        printf("start server ...\n");
        Singleton<GameEngine>::get_mutable_instance().Initialize().Startup();

    }
    else if (strncmp(type, "client", sizeof("client")) == 0)
    {
        printf("start client ...\n");
        Singleton<RUDPClient>::get_mutable_instance().Initialize(0x11223344, htonl(inet_addr("127.0.0.1")), 8888).Run();
    }
    else if (strncmp(type, "gm", sizeof("gm")) == 0)
    {
        printf("GM start ...\n");
        InitShmVector();
        AcceptStdInput();
    }
    else if (strncmp(type, "testaddr", sizeof("testaddr")) == 0)
    {
        NetAddress addr(9, 134, 22, 167, 8888);
        printf("IP: %u.%u.%u.%u, PORT: %u\n", addr.GetA(),
               addr.GetB(), addr.GetC(), addr.GetD(), addr.GetPort());
    }
    else
    {
        Codecode();
    }

    DestroyShmVector();

    return 0;
}