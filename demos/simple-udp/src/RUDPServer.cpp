#include "RUDPServer.h"
#include "Commands.h"
#include "Session.h"
#include "Singleton.h"
#include "NetMessage.h"

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
        return;
    }

    if (static_cast<size_t>(recv_bytes) < sizeof(NetHeader))
    {
        printf("recv bytes is not match the header, header.len: %lu, recv.len: %ld\n",
               sizeof(NetHeader), recv_bytes);
        return;
    }

    NetHeader header;
    BufferReader reader(socket_recv_buffer, recv_bytes);
    header.Deserialize(reader);

    if (OnValidate(header) == false)
    {
        printf("validate header failed\n");
        return;
    }

    if (reader.IsReadDone())
    {
        printf("There is no data content of the protocol\n");
        return;
    }

    // find a session to process rawpackage
    Session *session = Singleton<SessionManager>::get_mutable_instance().GetSession(header.sid);

    // 在非START协议的情况下SESSION为空是异常情况
    if (session == nullptr && header.cmd != ENetCommandID::NET_CMD_START)
    {
        printf("not found the session by sid: %u\n", header.sid);
        return;
    }

    // 如果是START则新创建一个SESSION
    if (header.cmd == ENetCommandID::NET_CMD_START)
    {
        // Create a new session
        session = Singleton<SessionManager>::get_mutable_instance().CreateSession(this);
    }

    // 这里SESSION为空是异常情况
    if (session == nullptr)
    {
        printf("get or create session failed \n");
        return;
    }

    session->SetClientAddress(from_addr);

    ssize_t ret = 0;
    // -------------------------- for test
    // NetMessageHeader header2;
    // header2.cmd = ENetCommandID::NET_CMD_START;
    // header2.appid = appid;

    // StartResponse resp;
    // resp.guid = pkg.guid;
    // resp.sid = 1;

    // BufferWriter writer;
    // header2.Serialize(writer);
    // resp.Serialize(writer);

    // ret = svr_socket.SendTo(from_addr, writer.InternalBuffer(), writer.Length());
    // --------------------------

    session->CommandDispatcher(header.cmd, reader);

    printf("after command dispatcher, cmd: %u, current session_num: %d, ret: %ld\n", header.cmd,
           Singleton<SessionManager>::get_mutable_instance().Count(), ret);
}

bool RUDPServer::OnValidate(const NetHeader &header)
{
    return true;
}

// [Obsolete]
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
        if (recv_bytes < (ssize_t)sizeof(NetHeader))
        {
            printf("packet len is too small , ret: %ld\n", recv_bytes);
            continue;
        }

        busy = true;

        // 解析网络包头
        BufferReader netreader(socket_recv_buffer, recv_bytes);
        NetHeader header;
        header.Deserialize(netreader);

        // header.PrintString();

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
            printf("illegal package, there is no data to read after NetHeader\n");
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
