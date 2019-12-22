#include "RUDPClient.h"

using namespace yinpsoft;

int RUDPClient::Initialize(uint32_t appid, unsigned int address,
                           unsigned short port, int32_t timeout)
{
    int result = cli_socket.Open();
    if (result < 0)
    {
        return result;
    }

    cli_socket.SetNonBlock();
    svr_address.SetAddress(address);
    svr_address.SetPort(port);

    application_id = appid;
    wait_server_timeout = timeout;

    return 0;
}

void RUDPClient::Run()
{
    // init packet

    int ret = 0;

    while (true)
    {
        std::string cli_command;

        // 直接使用std::cin << cli_command会导致命令中的空格而截断
        getline(std::cin, cli_command);

        std::transform(cli_command.begin(), cli_command.end(), cli_command.begin(), ::tolower);
        if (cli_command == "quit")
        {
            printf("quit the client of appid[%u]\n", application_id);
            break;
        }

        memset(request_packet, 0, MAX_PACKET_SIZE);

        SerializeData(cli_command.c_str(), cli_command.length());

        printf("before send, packet_size: %lu\n", pack_size);

        // ----------------------------------
        // 向服务器发包
        ret = cli_socket.SendTo(svr_address, request_packet, pack_size);
        // ----------------------------------

        printf("after send data to server, sent_len: %d, request_packet.len: %u\n", ret, pack_size);

        // 处理服务器回包
        memset(response_packet, 0, MAX_PACKET_SIZE);

        ssize_t recv_bytes = 0;
        auto recv_start = std::chrono::high_resolution_clock::now();
        while (true)
        {
            // ----------------------------------
            // 接收服务器回包
            recv_bytes = cli_socket.RecvFrom(svr_address, response_packet, MAX_PACKET_SIZE);
            // ----------------------------------

            if (recv_bytes < 0 && errno == EAGAIN)
            {
                auto recv_end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<int32_t> duration =
                    std::chrono::duration_cast<std::chrono::duration<int32_t>>(recv_end - recv_start);
                if (duration.count() >= wait_server_timeout)
                {
                    printf("waiting server for responsing timeout, elapsed: %d\n", duration.count());
                    break;
                }

                continue;
            }

            if (recv_bytes == 0 || errno != EAGAIN)
            {
                printf("recv server response failed, errno: %d\n", errno);
                break;
            }

            DumpPacket(response_packet, recv_bytes);

            // 检查网络包长度是否合法
            if (recv_bytes < (ssize_t)sizeof(NetMsgHeader))
            {
                printf("packet len is too small , ret: %ld\n", recv_bytes);
                break;
            }

            // 检查网络包是否来自合法客户端
            unsigned int from_appid = *(unsigned int *)response_packet;
            if (from_appid != application_id)
            {
                printf("this is no the packet from authenticated client, from_appid: %u\n", from_appid);
                break;
            }

            printf("[SERVER RESP] - %s\n", response_packet + sizeof(NetMsgHeader));

            break;
        }
    }

    Stop();
}

void RUDPClient::SerializeData(const char *payload, size_t len)
{
    printf("before serialize: payload: %s, payload.len: %lu\n", payload, len);
    pack_size = 0;

    // pack application_id of header
    for (uint32_t i = pack_size; i < pack_size + sizeof(application_id); i++)
    {
        request_packet[i] = (application_id >> i * 8) & 0xff;
    }
    pack_size += sizeof(application_id);

    printf("after pack appid, len: %lu, idx: %u\n", sizeof(request_packet), pack_size);

    DumpPacket(request_packet, pack_size);

    // pack sequence of header
    for (uint32_t i = pack_size; i < pack_size + sizeof(seq); i++)
    {
        request_packet[i] = (seq >> i * 8) & 0xff;
    }
    pack_size += sizeof(seq);

    printf("after pack seq, len: %lu, idx: %u\n", sizeof(request_packet), pack_size);

    DumpPacket(request_packet, pack_size);

    // pack data
    memcpy(request_packet + pack_size, payload, len);
    pack_size += len;

    printf("after pack seq, len: %lu, idx: %u\n", sizeof(request_packet), pack_size);
    DumpPacket(request_packet, pack_size);

    printf("after serialize request data: %s, len: %lu\n", request_packet, pack_size);
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
}