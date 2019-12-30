#include "RUDPServer.h"

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

void RUDPServer::Tick(int fps)
{
    // 1. 接收并解析网络包并存入接收队列
    RecvBytesFromNetwork();

    // 2. 从发送队列将包发送到网络
    // SendPackageToNetwork();
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
