#include "RUDPClient.h"
#include "Commands.h"
#include "NetMessage.h"
#include "boost/thread.hpp"

using namespace yinpsoft;

RUDPClient &RUDPClient::Initialize(uint32_t appid, unsigned int address,
                                   unsigned short port, bool shm,
                                   int32_t interval_ms)
{
    enable_shm = shm;
    int result = cli_socket.Open();
    if (result < 0)
    {
        return *this;
    }

    cli_socket.SetNonBlock();
    svr_address.SetAddress(address);
    svr_address.SetPort(port);

    application_id = appid;
    interval = interval_ms;
    client_tick = 0;

    if (enable_shm == true)
    {
        shared_memory_object::remove("SHM_COMMAND_LIST");
        managed_shared_memory segment(open_or_create, "SHM_COMMAND_LIST", 65536);
        const ShmAllocator alloc_inst(segment.get_segment_manager());
        shm_command_list = segment.find_or_construct<shm_vector>("COMMAND_LIST")(alloc_inst);
    }

    printf("finish client init\n");

    return *this;
}

void RUDPClient::AddClientPackage(BufferWriter &writer)
{
    ClientPackage pkg;
    memcpy(pkg.buff, writer.Raw().Buffer(), writer.Raw().Length());
    pkg.len = writer.Raw().Length();

    pending_send_list.emplace_back(pkg);
}

void RUDPClient::PerformQuit()
{
    NetMessageHeader header;
    header.cmdid = ENetCommandID::NET_CMD_QUIT;
    header.appid = application_id;

    BufferWriter writer;
    size_t len = header.Serialize(writer);

    printf("after serialize len: %lu\n", len);

    AddClientPackage(writer);
}

void RUDPClient::PerformHeartbeat()
{
    NetMessageHeader header;
    header.cmdid = ENetCommandID::NET_CMD_HEARTBEAT;
    header.appid = application_id;

    BufferWriter writer;
    size_t len = header.Serialize(writer);

    printf("after serialize len: %lu\n", len);

    AddClientPackage(writer);
}

void RUDPClient::PerformData()
{
    BufferWriter writer;

    NetMessageHeader header;
    header.cmdid = ENetCommandID::NET_CMD_DATA;
    header.appid = application_id;
    size_t len = header.Serialize(writer);

    printf("after serialize NetMessageHeader len: %lu\n", len);

    RawPackage raw;
    memcpy(raw.pkg_buff, command_params.c_str(), command_params.length());
    raw.pkg_len = command_params.length();
    raw.fragment_count = 1;
    raw.fragment_idx = 0;
    len = raw.Serialize(writer);

    printf("after serialize RawPackage len: %lu\n", len);

    AddClientPackage(writer);
}

void RUDPClient::NetCommandDispatcher(const int32_t cmd)
{
    switch (cmd)
    {
    case ENetCommandID::NET_CMD_QUIT:
    {
        PerformQuit();
        break;
    }
    case ENetCommandID::NET_CMD_HEARTBEAT:
    {
        PerformHeartbeat();
        break;
    }
    case ENetCommandID::NET_CMD_DATA:
    {
        PerformData();
        break;
    }
    default:
    {
        printf("illegal cmdid: %d\n", cmd);
        break;
    }
    }
}

void RUDPClient::OnInput()
{
    std::string input_str;
    getline(std::cin, input_str);
    std::transform(input_str.begin(), input_str.end(), input_str.begin(), ::tolower);

    if ("quit" == input_str)
    {
        local_command_list.push_back(ENetCommandID::NET_CMD_QUIT);
    }
    else
    {
        command_params = "";
        local_command_list.push_back(ENetCommandID::NET_CMD_DATA);
        command_params = input_str;
    }

    // TODO: other commands

    printf("after push command: %s, command count: %lu\n", input_str.c_str(), local_command_list.size());
}

// void RUDPClient::NetCommandDispacher(const std::string &command)
// {
//     ENetCommandID cmd = ENetCommandID::NET_CMD_UNDFINED;

//     cmd = COMMAND_MAP[command];

//     NetCommandDispacher(cmd);
// }

void RUDPClient::OnCommandDispatch()
{
    if (enable_shm == true)
    {
        if (shm_command_list == nullptr)
        {
            printf("not found command list\n");
            return;
        }

        for (auto itr = shm_command_list->begin(); itr != shm_command_list->end(); ++itr)
        {
            int32_t cli_command = *itr;
            NetCommandDispatcher(cli_command);
            printf("after dispatch command: %d, pending_send_list.size: %lu\n",
                   *itr, pending_send_list.size());
        }
    }
    else
    {
        for (const ENetCommandID &cmd : local_command_list)
        {
            NetCommandDispatcher(cmd);
            printf("after dispatch command: %d, pending_send_list.size: %lu\n",
                   cmd, pending_send_list.size());
        }

        local_command_list.clear();
    }
}

void RUDPClient::OnSend()
{
    int sent_count = 0;
    size_t total_count = pending_send_list.size();
    std::vector<ClientPackage>::iterator itr = pending_send_list.begin();
    for (; itr != pending_send_list.end();)
    {
        const ClientPackage &pkg = *itr;

        printf("before send, len: %lu, pkg.buff: \n", pkg.len);
        for (size_t i = 0; i < pkg.len; i++)
        {
            printf("%2x ", *(pkg.buff + i));
        }
        printf("\n");

        // ----------------------------------
        // 向服务器发包
        ssize_t ret = cli_socket.SendTo(svr_address, pkg.buff, pkg.len);
        // ----------------------------------

        printf("after send, ret: %ld\n", ret);
        itr = pending_send_list.erase(itr);

        if (ret > 0)
        {
            sent_count++;
        }
    }

    printf("sent_count: %d, total_count: %lu, rest: %lu\n",
           sent_count, total_count, pending_send_list.size());
}

bool RUDPClient::OnValidate(const NetMessageHeader &header)
{
    // if (header.appid != application_id)
    // {
    //     return false;
    // }

    return true;
}

void RUDPClient::OnRecv()
{
    uint8_t recv_buff[MAX_RAW_PACKAGE_SIZE];

    // ----------------------------------
    // 接收服务器回包
    ssize_t recv_bytes = cli_socket.RecvFrom(svr_address, (void *)recv_buff, MAX_RAW_PACKAGE_SIZE);
    // ----------------------------------

    if (recv_bytes <= 0)
    {
        printf("recv failed, recv_bytes: %ld\n", recv_bytes);
        return;
    }

    BufferReader reader((const uint8_t *)recv_buff, recv_bytes);
    NetMessageHeader header;
    header.Deserialize(reader);

    if (OnValidate(header) == false)
    {
        printf("header is invalid\n");
        return;
    }

    RawPackage pkg;
    pkg.Deserialize(reader);
    pending_recv_list.emplace_back(pkg);
}

void RUDPClient::OnUpdate()
{
    for (int32_t i = 0; i < static_cast<int32_t>(pending_recv_list.size()); i++)
    {
        const RawPackage &pkg = pending_recv_list[i];

        // Logic Process...
        printf("fragment_idx: %u, fragment_count: %u, pkg_len: %lu, pkg_content: %s\n",
               pkg.fragment_idx, pkg.fragment_count, pkg.pkg_len, pkg.pkg_buff);
    }

    pending_recv_list.clear();
}

void RUDPClient::Run()
{
    printf("the client start to run ...\n");
    std::chrono::milliseconds tick_interval(interval);

    while (true)
    {
        // 1. 客户端输入
        if (enable_shm == false)
        {
            printf("waiting input: \n");
            OnInput();
        }
        // 2. 分发命令
        OnCommandDispatch();
        printf("after OnCommandDispatch\n");
        // 3. 发送数据
        OnSend();
        printf("after OnSend\n");
        // 4. 接收数据
        OnRecv();
        printf("after OnRecv\n");
        // 5. 逻辑处理
        OnUpdate();
        printf("after OnUpdate\n");

        std::this_thread::sleep_for(tick_interval);
        client_tick++;
        seq++;
    }

    Stop();
}

void RUDPClient::DumpBuffer(RawBuffer &buff)
{
    buff.Rewind();
    DumpPacket((const char *)(buff.Buffer()), buff.Length());
}

void RUDPClient::DumpPacket(const char *packet, size_t plen)
{
    for (int i = 0; i < static_cast<int>(plen); i++)
    {
        if (i % 32 == 0)
        {
            printf("\n");
        }
        printf("%2x ", packet[i]);
    }

    printf("\n");
}

void RUDPClient::Stop()
{
    cli_socket.Close();
    shared_memory_object::remove("SHM_COMMAND_LIST");
}