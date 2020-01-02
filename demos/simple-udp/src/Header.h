#pragma once

#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <cfloat>
#include <cinttypes>
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>

#include <sys/ioctl.h>
#include <alloca.h>
#include <netdb.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC 2
#define PLATFORM_UNIX 3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
#include <winsock2.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
#pragma comment(lib, "wsock32.lib")
#endif

#define MAX_PACKET_SIZE 496 // exclude packet header
#define APPID 0x11223344
#define TIME_PER_TICK 1000

using namespace std;

static const unsigned int appid = APPID;
static const unsigned short port = 8800;
// static const char* SERVER_IPADDR = "127.0.0.1";
