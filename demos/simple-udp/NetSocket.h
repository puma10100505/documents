#pragma once

#include "NetAddress.h"

class NetSocket final
{
public:
    NetSocket();
    ~NetSocket() { is_open = false; }

    int Open();
    int Bind(const NetAddress &svraddr);
    inline bool IsOpen() const
    {
        return is_open;
    }

    void Close();
    void SetNonBlock();
    ssize_t SendTo(const NetAddress &dst, const void *data, size_t data_len);
    ssize_t RecvFrom(NetAddress &src, void *data, size_t data_len);

private:
    bool is_open = false;
    int handler;
};