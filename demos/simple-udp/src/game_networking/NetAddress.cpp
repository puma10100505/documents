#include "NetAddress.h"
#include "Header.h"

using namespace yinpsoft;

NetAddress::NetAddress(unsigned char a, unsigned char b,
                       unsigned char c, unsigned char d, unsigned short p)
{
    address = (a << 24) | (b << 16) | (c << 8) | d;
    port = p;
}

NetAddress::NetAddress(unsigned int addr, unsigned short p)
{
    address = addr;
    port = p;
}

NetAddress::NetAddress(unsigned short p)
{
    address = INADDR_ANY;
    port = p;
}

unsigned char NetAddress::GetA() const
{
    return (address & 0xff000000) >> 24;
}

unsigned char NetAddress::GetB() const
{
    return (address & 0x00ff0000) >> 16;
}

unsigned char NetAddress::GetC() const
{
    return (address & 0x0000ff00) >> 8;
}

unsigned char NetAddress::GetD() const
{
    return (address & 0x000000ff);
}