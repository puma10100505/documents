#include "NetSocket.h"

using namespace yinpsoft;

NetSocket::NetSocket()
{
    // Nothing
}

int NetSocket::Open()
{
    handler = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (handler < 0)
    {
        printf("create udp socket failed, handler: %d, errno: %d, reason: %s\n",
               handler, errno, strerror(errno));
        return handler;
    }

    is_open = true;

    return 0;
}

int NetSocket::Bind(const NetAddress &svraddr)
{
    if (IsOpen() == false)
    {
        printf("the socket is not opened, cannot invoke bind method\n");
        return -1;
    }

    struct sockaddr_in address_info;
    address_info.sin_family = AF_INET;
    address_info.sin_addr.s_addr = INADDR_ANY;
    address_info.sin_port = htons(svraddr.GetPort());

    if (::bind(handler, (struct sockaddr *)&address_info, sizeof(address_info)) < 0)
    {
        printf("failed to bind server address\n");
        return -2;
    }

    return 0;
}

void NetSocket::Close()
{
    ::close(handler);
    is_open = false;
    printf("socket handler was shutdown\n");
}

void NetSocket::SetNonBlock()
{
    if (IsOpen() == false)
    {
        printf("the socket is not opened, cannot invoke set_nonblock method\n");
        return;
    }

    int non_blocking = 1;
    if (::fcntl(handler, F_SETFL, O_NONBLOCK, non_blocking) == -1)
    {
        printf("failed to set non-blocking\n");
        return;
    }
}

ssize_t NetSocket::SendTo(const NetAddress &dst, const void *data, size_t data_len)
{
    if (IsOpen() == false)
    {
        printf("the socket is not opened, cannot invoke sendto method\n");
        return -1;
    }

    struct sockaddr_in address_info;
    address_info.sin_family = AF_INET;
    address_info.sin_addr.s_addr = htonl(dst.GetAddress());
    address_info.sin_port = htons(dst.GetPort());

    printf("before send2, addr: %u, port: %u\n", dst.GetAddress(), dst.GetPort());
    ssize_t sent_bytes = ::sendto(handler, data, data_len, 0,
                                  (struct sockaddr *)&address_info, sizeof(sockaddr_in));
    if (sent_bytes != static_cast<ssize_t>(data_len))
    {
        printf("failed to send data, the sent len is mismatch, sent: %lu, origin_len: %lu, errno: %d, msg: %s, handler: %d\n",
               sent_bytes, data_len, errno, strerror(errno), handler);
        return -2;
    }

    return sent_bytes;
}

ssize_t NetSocket::RecvFrom(NetAddress &src, void *data, size_t data_len)
{
    if (IsOpen() == false)
    {
        printf("the socket is not opened, cannot invoke recvfrom method\n");
        return -1;
    }

    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);

    ssize_t recv_bytes = ::recvfrom(handler, (char *)data, data_len, 0,
                                    (struct sockaddr *)&from, &from_len);

    if (recv_bytes <= 0)
    {
        return -2;
    }

    // 接收失败不能到这些，否则会将svraddress改为不正确的值
    src.SetAddress(ntohl(from.sin_addr.s_addr));
    src.SetPort(ntohs(from.sin_port));

    return recv_bytes;
}