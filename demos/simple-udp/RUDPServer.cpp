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

void RUDPServer::Tick(int fps)
{
    svr_tick = 0;

    std::chrono::milliseconds ms_duration(1000 / fps);

    while (true)
    {
        std::this_thread::sleep_for(ms_duration);
        svr_tick++;

        auto tick_start = std::chrono::high_resolution_clock::now();
        NetAddress client_addr;

        memset(raw_data, 0, MAX_PACKET_SIZE);

        //------------------------------------
        // 从网络接收对端的数据
        ssize_t recv_bytes = svr_socket.RecvFrom(client_addr, (void *)raw_data, MAX_PACKET_SIZE);
        //------------------------------------

        if (recv_bytes < 0 && errno == EAGAIN)
        {
            continue;
        }

        if (recv_bytes == 0)
        {
            printf("client is closed\n");
            break;
        }

        last_client_packet_timestamp = std::chrono::high_resolution_clock::now();

        // 检查包内容
        // 检查网络包长度是否合法
        if (recv_bytes < (ssize_t)sizeof(NetMsgHeader))
        {
            printf("packet len is too small , ret: %ld\n", recv_bytes);
            continue;
        }

        // 检查网络包是否来自合法客户端(appid检查)
        uint32_t from_appid = *(uint32_t *)raw_data;
        if (from_appid != appid)
        {
            printf("this is no the packet from authenticated client, from_appid: %u\n", from_appid);
            continue;
        }

        // 序列号检查
        uint32_t latest_remote_seq = *(uint32_t *)(raw_data + sizeof(uint32_t));
        if (latest_remote_seq > remote_seq)
        {
            remote_seq = latest_remote_seq;
        }

        DumpPacket(raw_data, sizeof(raw_data));

        memset(payload_data, 0, MAX_PACKET_SIZE);

        // 提取网络包的数据部分
        memcpy(payload_data, raw_data + sizeof(NetMsgHeader), sizeof(raw_data) - sizeof(NetMsgHeader));

        printf("[SERVER] - After receive data from [address: %u.%u.%u.%u, port: %u], [Content: %s][svr_tick: %ld]\n",
               client_addr.GetA(), client_addr.GetB(), client_addr.GetC(), client_addr.GetD(), client_addr.GetPort(), payload_data, svr_tick);

        // 向客户端回包
        memset(raw_data, 0, MAX_PACKET_SIZE);
        SerializeData(payload_data, strlen(payload_data));

        //------------------------------------
        // 向客户端回包
        svr_socket.SendTo(client_addr, raw_data, pack_size);
        //------------------------------------

        auto tick_end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> frame_elapsed = tick_end - tick_start;

        seq++;

        printf("[SERVER] - frame_elapsed: %f\n", frame_elapsed.count());
    }
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
