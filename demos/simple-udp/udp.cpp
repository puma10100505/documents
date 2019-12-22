#include "Header.h"

#include "NetSocket.h"
#include "Singleton.h"
#include "RUDPClient.h"
#include "RUDPServer.h"

#define PACKET_LEN 500

using namespace yinpsoft;
// TODO: recv & send packet timeout, calc in tick

int simclient()
{
    // 1. create the udp socket
    int handle = socket(AF_INET, SOCK_DGRAM, 0);
    if (handle < 0)
    {
        printf("create udp socket failed\n");
        return -1;
    }

    // 2. prepare the socket address
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    // address.sin_addr.s_addr = inet_addr(SERVER_IPADDR);
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // 3. set non-blocking
    int non_blocking = 1;
    if (fcntl(handle, F_SETFL, O_NONBLOCK, non_blocking) == -1)
    {
        printf("failed to set non-blocking\n");
        return -3;
    }

    char packet_data[PACKET_LEN] = "this is netowrking payload\n";

    // 5. sending bytes
    size_t sent_bytes = sendto(handle, (const void *)packet_data, strlen(packet_data),
                               0, (struct sockaddr *)&address, sizeof(address));
    if (sent_bytes != strlen(packet_data))
    {
        printf("failed to send bytes, sent_byts: %lu, packet.len: %lu\n", sent_bytes, strlen(packet_data));
        return -4;
    }

    printf("after send networking bytes, sent len: %lu\n", sent_bytes);

    close(handle);

    return 0;
}

void new_client()
{
    NetSocket socket;

    int ret = socket.Open();
    if (ret < 0)
    {
        printf("open socket failed, ret: %d\n", ret);
        return;
    }

    NetAddress address_info(9, 134, 22, 167, 9000);

    char packet[MAX_PACKET_SIZE];

    // init packet
    memset(packet, 0, MAX_PACKET_SIZE);

    char payload[] = "PAY LOAD DATA FROM CLIENT...";

    for (int i = 0; i < 4; i++)
    {
        packet[i] = (appid >> i * 8) & 0xff;
    }

    // pack data
    memcpy(packet + sizeof(NetMsgHeader), (const char *)payload, strlen(payload));

    for (int i = 0; i < MAX_PACKET_SIZE; i++)
    {
        if (i % 32 == 0)
        {
            printf("\n");
        }
        printf("%2x ", packet[i]);
    }

    printf("before send, packet_size: %lu\n", strlen(packet));

    ret = socket.SendTo(address_info, (const void *)packet, strlen(packet));

    printf("after send data to server, sent_len: %d\n", ret);

    socket.Close();

    return;
}

void new_server()
{
    NetSocket socket;
    int64_t svr_tick = 0;

    std::chrono::milliseconds ms_duration(TIME_PER_TICK);

    int ret = socket.Open();
    if (ret < 0)
    {
        printf("open socket failed, ret: %d\n", ret);
        return;
    }

    NetAddress address_info(9000);
    socket.Bind(address_info);
    socket.SetNonBlock();

    while (true)
    {
        std::this_thread::sleep_for(ms_duration);
        svr_tick++;
        printf("tick: %ld\n", svr_tick);

        auto tick_start = std::chrono::high_resolution_clock::now();
        NetAddress client_addr;
        char raw_data[MAX_PACKET_SIZE];
        char payload_data[MAX_PACKET_SIZE];

        memset(raw_data, 0, MAX_PACKET_SIZE);
        memset(payload_data, 0, MAX_PACKET_SIZE);

        ssize_t ret = socket.RecvFrom(client_addr, (void *)raw_data, PACKET_LEN);
        if (ret < 0 && errno == EAGAIN)
        {
            continue;
        }

        if (ret == 0)
        {
            printf("client is closed\n");
            break;
        }

        if (ret < (ssize_t)sizeof(NetMsgHeader))
        {
            printf("packet len is too small, ret: %ld\n", ret);
            continue;
        }

        unsigned int from_appid = *(unsigned int *)raw_data;

        if (from_appid != appid)
        {
            printf("this is not the packet from authenticated client, from_appid: %u\n", from_appid);
            continue;
        }

        memcpy(payload_data, raw_data + sizeof(unsigned int), strlen(raw_data) - sizeof(unsigned int));

        printf("after recv from client(address: %u.%u.%u.%u, port: %u), datalen: %ld, data: [%s]\n",
               client_addr.GetA(), client_addr.GetB(), client_addr.GetC(), client_addr.GetD(),
               client_addr.GetPort(), ret, payload_data);

        auto tick_end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> frame_elapsed = tick_end - tick_start;

        printf("frame_elapsed: %f\n", frame_elapsed.count());
    }

    socket.Close();

    return;
}

int simserver()
{
    // 1. create the udp socket
    int handle = socket(AF_INET, SOCK_DGRAM, 0);
    if (handle < 0)
    {
        printf("create udp socket failed\n");
        return -1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // 3. bind the handle with address
    if (bind(handle, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("failed to bind socket \n");
        return -2;
    }

    // 2. make non-blocking
    int non_blocking = 1;
    if (fcntl(handle, F_SETFL, O_NONBLOCK, non_blocking) == -1)
    {
        printf("failed to set non-blocking\n");
        return -2;
    }

    char packet_data[PACKET_LEN];

    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);

    while (true)
    {
        memset(packet_data, 0, PACKET_LEN);
        int bytes = recvfrom(handle, (void *)packet_data, PACKET_LEN, 0,
                             (struct sockaddr *)&from, &from_len);
        if (bytes < 0 && errno == EAGAIN)
        {
            continue;
        }

        if (bytes == 0)
        {
            printf("none of bytes was recv, bytes: %d, errno: %d, reson: %s\n",
                   bytes, errno, strerror(errno));
            break;
        }

        printf("after recv from client, len: %d, content: %s", bytes, packet_data);

        // unsigned int from_address = ntohl(from.sin_addr.s_addr);
        // unsigned int from_port = ntohs(from.sin_port);
    }

    printf("after recv all the data from networking\n");
    close(handle);

    return 0;
}

int allinone()
{
    // 1. create socket
    int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (handle < 0)
    {
        printf("create udp socket failed\n");
        return -1;
    }

    // 2. prepare address
    // unsigned int a = 9;
    // unsigned int b = 134;
    // unsigned int c = 22;
    // unsigned int d = 167;
    // unsigned short port = 30000;
    // unsigned int address_val = (a << 24) | (b << 16) | (c << 8) | d;

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("9.134.22.167");
    address.sin_port = htons(port);

    // 3. bind the handle with address
    if (bind(handle, (struct sockaddr *)&address, sizeof(sockaddr_in)) < 0)
    {
        printf("failed to bind socket \n");
        return -2;
    }

    // 4. make non-blocking
    int non_blocking = 1;
    if (fcntl(handle, F_SETFL, O_NONBLOCK, non_blocking) == -1)
    {
        printf("failed to set non-blocking\n");
        return -2;
    }

    // 5. prepare sending data
    char send_data[PACKET_LEN];
    memset(send_data, '\0', PACKET_LEN);
    std::string data = "this is netowrking payload";
    memcpy(send_data, data.c_str(), data.length());

    // 6. sending bytes
    size_t sent_bytes = ::sendto(handle, (char *)send_data, data.length(), 0, (sockaddr *)&address, sizeof(sockaddr_in));
    if (sent_bytes != data.length())
    {
        printf("failed to send bytes\n");
        return -4;
    }

    printf("after send networking bytes, sent len: %lu\n", sent_bytes);

    // 7. recv packets
    while (true)
    {
        unsigned char recv_data[PACKET_LEN];
        memset(recv_data, '\0', PACKET_LEN);
        unsigned int max_packet_size = sizeof(recv_data);

        sockaddr_in from;
        socklen_t from_len = sizeof(from);

        size_t bytes = ::recvfrom(handle, (char *)recv_data, max_packet_size, 0, (sockaddr *)&from, &from_len);
        if (bytes <= 0)
        {
            printf("none of bytes was recv, bytes: %lu, errno: %d, reson: %s\n", bytes, errno, strerror(errno));
            break;
        }

        printf("after recv from client, len: %lu, content: %s\n", bytes, recv_data);

        // unsigned int from_address = ntohl(from.sin_addr.s_addr);
        // unsigned int from_port = ntohs(from.sin_port);
    }

    printf("after recv all the data from networking\n");
    close(handle);

    return 0;
}

void new_client_v2()
{
    Singleton<RUDPClient>::get_mutable_instance().Initialize(0x11223344, htonl(inet_addr("127.0.0.1")), 8888);
    Singleton<RUDPClient>::get_mutable_instance().Run();
}

void new_server_v2()
{
    Singleton<RUDPServer>::get_mutable_instance().Initialize(0x11223344, 8888);
    Singleton<RUDPServer>::get_mutable_instance().Tick();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("the paramter num is illegal\n");
        return -1;
    }

    char *type = argv[1];

    if (strncmp(type, "server", sizeof("server")) == 0)
    {
        printf("start server ...\n");
        // simserver();
        // new_server();
        new_server_v2();
    }
    else if (strncmp(type, "client", sizeof("client")) == 0)
    {
        printf("start client ...\n");
        // simclient();
        // new_client();
        new_client_v2();
    }
    else if (strncmp(type, "allinone", sizeof("allinone")) == 0)
    {
        printf("start allinone ...\n");
        allinone();
    }
    else if (strncmp(type, "testaddr", sizeof("testaddr")) == 0)
    {
        NetAddress addr(9, 134, 22, 167, 8888);
        printf("IP: %u.%u.%u.%u, PORT: %u\n", addr.GetA(),
               addr.GetB(), addr.GetC(), addr.GetD(), addr.GetPort());
    }
    else
    {
        printf("illegal type of process\n");
        return -2;
    }

    return 0;
}