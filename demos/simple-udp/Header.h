#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <inttypes.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <alloca.h>
#include <chrono>
#include <thread>

#define MAX_PACKET_SIZE 496 // exclude header

#define APPID 0x11223344
#define TIME_PER_TICK 1000

const unsigned int appid = APPID;

typedef struct stNetMsgHeader
{
    unsigned int appid; // REF: https://gafferongames.com/post/virtual_connection_over_udp/
} NetMsgHeader;

typedef struct stNetMsg
{
    char payload[MAX_PACKET_SIZE];
} NetMsg;