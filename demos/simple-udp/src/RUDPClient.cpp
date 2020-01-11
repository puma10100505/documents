#include "RUDPClient.h"
#include "Commands.h"
#include "NetMessage.h"

#include "boost/thread/thread.hpp"
#include "boost/bind/bind.hpp"
#include "gameplay.pb.h"

using namespace yinpsoft;

static const chrono::milliseconds thread_interval(50);

void RUDPClient::Initialize(uint32_t appid, unsigned int address,
                                   unsigned short port, bool shm,
                                   int32_t interval_ms)
{
    enable_shm = shm;
    int result = cli_socket.Open();
    if (result < 0)
    {
        return ;
    }

    // cli_socket.SetNonBlock();
    svr_address.SetAddress(address);
    svr_address.SetPort(port);

    application_id = appid;
    interval = interval_ms;
    client_tick = 0;

    // if (enable_shm == true)
    // {
    //     shared_memory_object::remove("SHM_COMMAND_LIST");
    //     managed_shared_memory segment(open_or_create, "SHM_COMMAND_LIST", 65536);
    //     const ShmAllocator alloc_inst(segment.get_segment_manager());
    //     shm_command_list = segment.find_or_construct<shm_vector>("COMMAND_LIST")(alloc_inst);
    // }

    set_guid(10908259400932504);

    printf("finish client init\n");

    return ;
}

void RUDPClient::SendPackage(BufferWriter &writer)
{
    SendingPackage pkg;
    memcpy(pkg.buff, writer.Raw().Buffer(), writer.Raw().Length());
    pkg.len = writer.Raw().Length();

    pending_send_list.emplace_back(pkg);
}

void RUDPClient::PerformQuit()
{
    NetHeader header;
    header.cmd = ENetCommandID::NET_CMD_QUIT;
    header.sid = sid();

    QuitReq req;
    req.guid = guid();
    req.battle_id = battle_id();
    req.sid = sid();

    BufferWriter writer;
    header.Serialize(writer);
    req.Serialize(writer);

    SendPackage(writer);
}

void RUDPClient::PerformHeartbeat()
{
    NetMessageHeader header;
    header.cmdid = ENetCommandID::NET_CMD_HEARTBEAT;
    header.appid = application_id;

    BufferWriter writer;
    size_t len = header.Serialize(writer);

    printf("after serialize len: %lu\n", len);

    SendPackage(writer);
}

// void RUDPClient::PerformOpenWorld()
// {
//     NetHeader header;
//     header.cmd = ENetCommandID::NET_CMD_BUILD_WORLD;
//     header.sid = sid();

//     OpenWorldReq req;
//     req.guid = guid();
//     req.sid = sid();

//     BufferWriter writer;
//     header.Serialize(writer);
//     req.Serialize(writer);

//     SendPackage(writer);
// }

void RUDPClient::PerformPlayerEnter()
{
    NetHeader header;
    header.cmd = ENetCommandID::NET_CMD_PLAYER_ENTER;
    header.sid = sid();

    EnterReq req;
    req.guid = guid();
    req.sid = sid();

    BufferWriter writer;
    header.Serialize(writer);
    req.Serialize(writer);

    SendPackage(writer);
}

void RUDPClient::PerformStart()
{
    NetHeader header;
    header.cmd = ENetCommandID::NET_CMD_START;
    header.sid = 0;

    StartReq req;
    req.udid = 0;
    req.guid = guid();
    //req.battle_id = 17295764174200172546;

    BufferWriter writer;
    size_t len = header.Serialize(writer);
    printf("after serialize netheader, len: %lu\n", len);
    len = req.Serialize(writer);
    printf("after serialize startreq len: %lu\n", len);

    SendPackage(writer);
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

    SendPackage(writer);
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
    case ENetCommandID::NET_CMD_START:
    {
        PerformStart();
        break;
    }
    case ENetCommandID::NET_CMD_PLAYER_ENTER:
    {
        PerformPlayerEnter();
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

    do
    {
        if (command_map.find(input_str) != command_map.end())
        {
            local_command_list.push_back(command_map[input_str]);
            break;
        }

        command_params = "";
        local_command_list.push_back(ENetCommandID::NET_CMD_DATA);
        command_params = input_str;

    } while (false);

    // TODO: other commands

    printf("after push command: %s, command count: %lu\n", input_str.c_str(), local_command_list.size());
}

void RUDPClient::OnCommandDispatch()
{
    if (enable_shm == true)
    {
        // if (shm_command_list == nullptr)
        // {
        //     printf("not found command list\n");
        //     return;
        // }

        // for (auto itr = shm_command_list->begin(); itr != shm_command_list->end(); ++itr)
        // {
        //     int32_t cli_command = *itr;
        //     NetCommandDispatcher(cli_command);
        //     printf("after dispatch command: %d, pending_send_list.size: %lu\n",
        //            *itr, pending_send_list.size());
        // }
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

void RUDPClient::SendThread()
{
    while (running())
    {
        OnCommandDispatch();

        if (pending_send_list.size() <= 0)
        {
            std::this_thread::sleep_for(thread_interval);
        }
        else
        {
            OnSend();
        }
    }
}

void RUDPClient::OnSend()
{
    int sent_count = 0;
    size_t total_count = pending_send_list.size();
    std::vector<SendingPackage>::iterator itr = pending_send_list.begin();
    for (; itr != pending_send_list.end();)
    {
        const SendingPackage &pkg = *itr;

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

        printf("after send, ret: %ld, address: %s, port: %u\n", ret, svr_address.ToString().c_str(), svr_address.GetPort());
        itr = pending_send_list.erase(itr);

        if (ret > 0)
        {
            sent_count++;
        }
    }

    printf("sent_count: %d, total_count: %lu, rest: %lu\n",
           sent_count, total_count, pending_send_list.size());
}

bool RUDPClient::OnValidate(const NetHeader &header)
{
    return true;
}

void RUDPClient::RecvThread()
{
    while (running())
    {
        OnRecv();
        std::this_thread::sleep_for(thread_interval);
    }
}

void RUDPClient::OnRecv()
{
    uint8_t recv_buff[MAX_RAW_PACKAGE_SIZE];

    ssize_t recv_bytes = 0;

    printf("before waiting recv\n");
    // ----------------------------------
    // 接收服务器回包
    recv_bytes = cli_socket.RecvFrom(svr_address, (void *)recv_buff, MAX_RAW_PACKAGE_SIZE);
    // ----------------------------------

    printf("after recv from socket, len: %lu\n", recv_bytes);
    if (recv_bytes <= 0)
    {
        return;
    }

    BufferReader reader((const uint8_t *)recv_buff, recv_bytes);

    NetHeader header;
    header.Deserialize(reader);

    printf("reader.index: %lu, header.length: %lu\n", reader.Raw().Position(), sizeof(header));
    if (OnValidate(header) == false)
    {
        printf("header is invalid\n");
        return;
    }

    ReceivedPackage resp;
    resp.cmd = header.cmd;
    switch (header.cmd)
    {
    case ENetCommandID::NET_CMD_START:
    {
        StartResp pkg;
        pkg.Deserialize(reader);
        resp.package.start = pkg;
        break;
    }
    case ENetCommandID::NET_CMD_QUIT:
    {
        QuitResp pkg;
        pkg.Deserialize(reader);
        resp.package.quit = pkg;
        break;
    }
    case ENetCommandID::NET_CMD_OBJECT_SPAWN:
    {
        printf("NET_CMD_OBJECT_SPAWN, pkg.len: %lu, pkg.pos: %lu\n", reader.Raw().Length(), reader.Raw().Position());
        pb::PBGameObject go;
        reader.ReadProto(go);
        printf("after read proto of gameobject, go: %s\n", go.ShortDebugString().c_str());
        resp.go = go;
        break;  
    }
    case ENetCommandID::NET_CMD_OBJECT_REPLICATE:
    {
        printf("NET_CMD_OBJECT_REPLICATE, pkg.len: %lu, pkg.pos: %lu\n", reader.Raw().Length(), reader.Raw().Position());        
        break;
    }
    default:
        break;
    }
    pending_recv_list.emplace_back(std::move(resp));
}

void RUDPClient::UpdateThread()
{
    while (running())
    {
        if (pending_recv_list.size() <= 0)
        {
            std::this_thread::sleep_for(thread_interval);
        }
        else
        {
            OnUpdate();
        }
    }
}

void RUDPClient::OnUpdate()
{
    printf("on update in parent\n");
}

void RUDPClient::Run()
{
    set_running(true);

    printf("the client start to run ...\n");
    std::chrono::milliseconds tick_interval(interval);

    boost::thread send_thread(boost::bind(&RUDPClient::SendThread, this));
    send_thread.detach();

    boost::thread update_thread(boost::bind(&RUDPClient::UpdateThread, this));
    update_thread.detach();

    boost::thread recv_thread(boost::bind(&RUDPClient::RecvThread, this));
    recv_thread.detach();

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
    // TODO: 向服务器发送STOP包告知服务器当前客户端退出，需关闭SESSION以及CONNECTION
    cli_socket.Close();
    // shared_memory_object::remove("SHM_COMMAND_LIST");
}