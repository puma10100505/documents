#include "RUDPServer.h"

using namespace yinpsoft;


int RUDPServer::Initialize(uint32_t appid, unsigned short port)
{
    application_id = appid;
    int result = svr_socket.Open();
    if (result < 0) 
    {
        return result;
    }

    NetAddress address_info(port);
    svr_socket.Bind(address_info);
    svr_socket.SetNonBlock();

    return 0;
}

void RUDPServer::Tick(int fps) 
{
    svr_tick = 0;

    std::chrono::milliseconds ms_duration(1000/fps);

    while (true) 
    {
        std::this_thread::sleep_for(ms_duration);
        svr_tick++;

        printf("tick: %d\n", svr_tick);

        auto tick_start = std::chrono::high_resolution_clock::now();
        NetAddress client_addr;
        
        memset(raw_data, 0, MAX_PACKET_SIZE);
        memset(payload_data, 0, MAX_PACKET_SIZE);

        // 从网络接收对端的数据
        ssize_t recv_bytes = svr_socket.RecvFrom(client_addr, (void *)raw_data, MAX_PACKET_SIZE);
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

        // 检查网络包长度是否合法
        if (recv_bytes < (ssize_t)sizeof(NetMsgHeader)) 
        {
            printf("packet len is too small , ret: %ld\n", recv_bytes);
            continue;
        }

        // 检查网络包是否来自合法客户端
        unsigned int from_appid = *(unsigned int *)raw_data;
        if (from_appid != appid) 
        {
            printf("this is no the packet from authenticated client, from_appid: %u\n", from_appid);
            continue;
        }

        // 提取网络包的数据部分
        memcpy(payload_data, raw_data + sizeof(unsigned int), strlen(raw_data) - sizeof(unsigned int));

        printf("after recv from client(address: %u.%u.%u.%u, port: %u), datalen: %ld, data: [%s]\n",
               client_addr.GetA(), client_addr.GetB(), client_addr.GetC(), client_addr.GetD(),
               client_addr.GetPort(), recv_bytes, payload_data);

        // 向客户端回包
        memset(raw_data, 0, MAX_PACKET_SIZE);
        SerializeData("server received the data", strlen("server received the data"));

        svr_socket.SendTo(client_addr, raw_data, strlen(raw_data));

        auto tick_end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> frame_elapsed = tick_end - tick_start;

        printf("frame_elapsed: %f\n", frame_elapsed.count());
    }
}

void RUDPServer::SerializeData(const char* data, size_t len)
{
    for (int i = 0; i < 4; i++)
    {
        raw_data[i] = (application_id >> i * 8) & 0xff;
    }

    // pack data
    memcpy(raw_data + sizeof(NetMsgHeader), data, len);

    memcpy(raw_data + sizeof(NetMsgHeader) + len, "<SVR RETURNED>", strlen("<SVR RETURNED>"));
}

void RUDPServer::Stop()
{
    svr_socket.Close();
}