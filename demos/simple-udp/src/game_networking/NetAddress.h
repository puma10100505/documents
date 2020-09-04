#pragma once

#include "Header.h"

namespace yinpsoft
{
class NetAddress final
{
public:
    NetAddress() {}
    NetAddress(unsigned char a, unsigned char b, unsigned char c,
               unsigned char d, unsigned short p);
    NetAddress(unsigned int addr, unsigned short p);
    NetAddress(unsigned short p);

    unsigned char GetA() const;
    unsigned char GetB() const;
    unsigned char GetC() const;
    unsigned char GetD() const;

    inline unsigned short GetPort() const
    {
        return port;
    }

    inline unsigned int GetAddress() const
    {
        return address;
    }

    inline void SetAddress(unsigned int addr)
    {
        address = addr;
    }

    inline void SetPort(unsigned short p)
    {
        port = p;
    }

    inline const std::string &ToString()
    {
        address_desc = "";
        return address_desc.append(to_string(GetA())).append(".").append(to_string(GetB())).append(".").append(to_string(GetC())).append(".").append(to_string(GetD()));
    }

private:
    unsigned int address;
    unsigned short port;
    std::string address_desc;
};
}; // namespace yinpsoft