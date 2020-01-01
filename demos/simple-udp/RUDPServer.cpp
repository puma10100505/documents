#include "RUDPServer.h"
#include "Commands.h"
#include "Session.h"
#include "Singleton.h"

using namespace yinpsoft;

RUDPServer &RUDPServer::Initialize(uint32_t appid, unsigned short port)
{
    application_id = appid;
    int result = svr_socket.Open();
    if (result < 0)
    {
        return *this;
    }

    NetAddress address_info(port);
    svr_socket.Bind(address_info);
    svr_socket.SetNonBlock();

    return *this;
}

void RUDPServer::OnRecvBytes()
{
    uint8_t socket_recv_buffer[DEFAULT_MSS];

    NetAddress from_addr;
    ssize_t recv_bytes = svr_socket.RecvFrom(from_addr, (void *)socket_recv_buffer, DEFAULT_MSS);

    if (recv_bytes <= 0)
    {
        //printf("recv from client failed, recv_bytes: %ld\n", recv_bytes); 
        return;
    }

    if (static_cast<size_t>(recv_bytes) < sizeof(NetMessageHeader))
    {
        printf("recv bytes is not match the header, header.len: %lu, recv.len: %ld\n", 
            sizeof(NetMessageHeader), recv_bytes);
        return;
    }

    NetMessageHeader header;
    BufferReader reader(socket_recv_buffer, recv_bytes);
    header.Deserialize(reader);

    if (OnValidate(header) == false)
    {
        printf("validate header failed\n");
        return ;
    }

    if (reader.IsReadDone())
    {
        printf("There is no data content of the protocol\n");
        return;
    }

    RawPackage pkg;
    pkg.Deserialize(reader);

    // find a session to process rawpackage
    Session* session = Singleton<SessionManager>::get_mutable_instance().GetSession(header.sid);
    if (session == nullptr && header.cmdid != ENetCommandID::NET_CMD_START) {
        printf("not found the session by sid: %u\n", header.sid);
        return;
    }

    if (header.cmdid == ENetCommandID::NET_CMD_START)
    {
        // Create a new session
        session = Singleton<SessionManager>::get_mutable_instance().CreateSession(pkg.guid());
    }
    
    if (session == nullptr)
    {
        printf("get or create session failed \n");
        return; 
    }

    session->CommandDispatcher(header.cmdid, pkg);
    
    printf("after command dispatcher, cmdid: %u\n", header.cmdid);
}

void RUDPServer::CommandDispatcher(uint8_t cmd, const RawPackage& pkg)
{
    switch (cmd)
    {
        case ENetCommandID::NET_CMD_QUIT:
        {
            HandleQuitMessage(pkg);
            break;
        }
        case ENetCommandID::NET_CMD_HEARTBEAT:
        {
            HandleHeartbeatMessage(pkg);
            break;
        }
        case ENetCommandID::NET_CMD_DATA:
        {
            HandleDataMessage(pkg);
            break;
        }
        case ENetCommandID::NET_CMD_START:
        {
            HandleStartMessage(pkg);
            break;
        }
        default:
        {
            printf("Unknown server command, cmd: %u\n", cmd);
            break;
        }
    }
}

void RUDPServer::HandleQuitMessage(const RawPackage& pkg)
{
    printf("handle quit\n");
}

void RUDPServer::HandleHeartbeatMessage(const RawPackage& pkg)
{
    printf("handle heartbeat\n");
}

void RUDPServer::HandleDataMessage(const RawPackage& pkg)
{
    printf("handle data\n");
}

void RUDPServer::HandleStartMessage(const RawPackage& pkg)
{
    printf("handle start\n");
}

bool RUDPServer::OnValidate(const NetMessageHeader& header)
{
    return true;
}

void RUDPServer::RecvBytesFromNetwork()
{
    uint8_t socket_recv_buffer[DEFAULT_MSS];

    bool busy = false;
    while (true)
    {
        busy = false;
        NetAddress from_addr;
        ssize_t recv_bytes = svr_socket.RecvFrom(from_addr, (void *)socket_recv_buffer, DEFAULT_MSS);

        if (recv_bytes < 0 && errno == EAGAIN)
        {
            continue;
        }

        // 客户端已关闭
        if (recv_bytes == 0)
        {
            printf("client is closed\n");
            break;
        }

        printf("after recv from client, len: %ld\n", recv_bytes);

        // 包头大小不合法
        if (recv_bytes < (ssize_t)sizeof(NetMessageHeader))
        {
            printf("packet len is too small , ret: %ld\n", recv_bytes);
            continue;
        }

        busy = true;

        // 解析网络包头
        BufferReader netreader(socket_recv_buffer, recv_bytes);
        NetMessageHeader header;
        header.Deserialize(netreader);

        header.PrintString();

        // TODO: 网络包头信息的一些检查验证逻辑
        // ...

        if (netreader.IsReadDone() == false)
        {
            // 正常情况下应该还可以读取
            RawPackage package;
            package.Deserialize(netreader);
            recv_queue.emplace_back(package);

            package.PrintString();

            printf("package push into queue, queue.len: %lu\n", recv_queue.size());
        }
        else
        {
            printf("illegal package, there is no data to read after NetMessageHeader\n");
            continue;
        }

        if (busy == false)
        {
            break;
        }
    }
}

void RUDPServer::Tick()
{
    OnRecvBytes();
}

void RUDPServer::SerializeData(const char *data, size_t len)
{
    // printf("sizeof1: %u, sizeof2: %u, appid: %u\n", sizeof(application_id), sizeof(uint32_t), application_id);
    pack_size = 0;

    // pack application_id of header
    for (uint32_t i = pack_size; i < pack_size + sizeof(application_id); i++)
    {
        raw_data[i] = (application_id >> i * 8) & 0xff;
    }
    pack_size += sizeof(application_id);

    // pack seq of header
    for (uint32_t i = pack_size; i < pack_size + sizeof(seq); i++)
    {
        raw_data[i] = (seq >> i * 8) & 0xff;
    }
    pack_size += sizeof(seq);

    // pack the ack of header
    for (uint32_t i = pack_size; i < pack_size + sizeof(remote_seq); i++)
    {
        raw_data[i] = (remote_seq >> i * 8) & 0xff;
    }
    pack_size += sizeof(remote_seq);

    // pack data
    memcpy(raw_data + pack_size, data, len);
    pack_size += len;

    memcpy(raw_data + pack_size, " <= [SVR RETURNED] ", sizeof(" <= [SVR RETURNED] "));
    pack_size += sizeof(" <= [SVR RETURNED] ");

    for (uint32_t i = pack_size; i < pack_size + sizeof(svr_tick); i++)
    {
        raw_data[i] = (svr_tick >> i * 8) & 0xff;
    }

    pack_size += sizeof(svr_tick);

    printf("after serialize, pack_size: %d\n", pack_size);
}

void RUDPServer::Stop()
{
    svr_socket.Close();
}

void RUDPServer::DumpPacket(const char *packet, size_t plen)
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
