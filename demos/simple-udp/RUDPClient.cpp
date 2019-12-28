#include "RUDPClient.h"

#include "NetMessage.h"

using namespace yinpsoft;

RUDPClient &RUDPClient::Initialize(uint32_t appid, unsigned int address,
                                   unsigned short port, int32_t timeout_sec)
{
    int result = cli_socket.Open();
    if (result < 0)
    {
        return *this;
    }

    cli_socket.SetNonBlock();
    svr_address.SetAddress(address);
    svr_address.SetPort(port);

    application_id = appid;
    wait_server_timeout = timeout_sec;

    return *this;
}

void RUDPClient::Run()
{
    // init packet
    int ret = 0;

    while (true)
    {
        std::string cli_command;
        getline(std::cin, cli_command);

        std::transform(cli_command.begin(), cli_command.end(), cli_command.begin(), ::tolower);
        if (cli_command == "quit")
        {
            printf("quit the client of appid[%u]\n", application_id);
            break;
        }

        memset(request_packet, 0, MAX_RAW_PACKAGE_SIZE);

        BufferWriter writer;
        SerializeData(cli_command.c_str(), cli_command.length(), writer);

        printf("before send, packet_size: %u\n", pack_size);

        // ----------------------------------
        // 向服务器发包
        ret = cli_socket.SendTo(svr_address, writer.Raw().Buffer(), writer.Raw().Length());
        // ----------------------------------

        printf("after send data to server, sent_len: %d, request_packet.len: %u\n", ret, pack_size);

        // 处理服务器回包
        memset(response_packet, 0, MAX_RAW_PACKAGE_SIZE);

        ssize_t recv_bytes = 0;
        auto recv_start = std::chrono::high_resolution_clock::now();

        // int32_t curr_try_times = 0;
        bool resp_successed = true;
        while (true)
        {
            // ----------------------------------
            // 接收服务器回包
            recv_bytes = cli_socket.RecvFrom(svr_address, response_packet, MAX_RAW_PACKAGE_SIZE);
            // ----------------------------------

            if (recv_bytes < 0 && errno == EAGAIN)
            {
                auto recv_end = std::chrono::high_resolution_clock::now();

                // 按秒计算的耗时
                std::chrono::duration<int32_t> duration =
                    std::chrono::duration_cast<std::chrono::duration<int32_t>>(recv_end - recv_start);

                if (duration.count() >= wait_server_timeout)
                {
                    printf("waiting server for responsing timeout, elapsed: %d\n", duration.count());
                    resp_successed = false;
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

            BufferReader reader(response_packet, recv_bytes);
            yinpsoft::NetMessageHeader header;
            yinpsoft::RawPackage net_pkg;

            header.Deserialize(reader);
            net_pkg.Deserialize(reader);

            // 检查网络包是否来自合法客户端
            //uint32_t from_appid = *(uint32_t *)response_packet;
            if (header.appid != application_id)
            {
                printf("this is no the packet from authenticated client, from_appid: %u\n", from_appid);
                break;
            }

            // 检查seq
            uint32_t latest_remote_seq = header.sequence; //*(uint32_t *)(response_packet + sizeof(uint32_t));
            if (latest_remote_seq > remote_seq)
            {
                remote_seq = latest_remote_seq;
            }

            uint32_t server_ack = header.ack; // *(uint32_t *)(response_packet + sizeof(uint32_t) * 2);

            printf("[SERVER RESP] - %s, [ACK: %u] [Client_SEQ: %u]\n", response_packet + sizeof(NetMsgHeader), server_ack, seq);

            seq++;

            break;
        }

        if (resp_successed == false)
        {
            break;
        }
    }

    Stop();
}

void RUDPClient::SerializeData(const char *payload, size_t len, BufferWriter &writer)
{
    printf("before serialize: payload: %s, payload.len: %lu\n", payload, len);

    yinpsoft::NetMessageHeader header;
    yinpsoft::RawPackage pkg;

    header.appid = application_id;
    header.sequence = seq;
    header.ack = seq;
    header.payload_size = len;

    pkg.pkg_len = len;
    memcpy(pkg.pkg_buff, payload, len);

    header.Serialize(writer);
    pkg.Serialize(writer);

    /*
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

    // pack ack of header
    for (uint32_t i = pack_size; i < pack_size + sizeof(seq); i++)
    {
        request_packet[i] = (seq >> i * 8) & 0xff;
    }
    pack_size += sizeof(seq);

    // pack data
    memcpy(request_packet + pack_size, payload, len);
    pack_size += len;

    */
    // printf("after pack seq, len: %lu, idx: %u\n", sizeof(request_packet), pack_size);
    DumpPacket(writer.Raw().Buffer(), writer.Raw().Length());
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