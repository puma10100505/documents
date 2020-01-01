#pragma once

enum ENetCommandID : int
{
    NET_CMD_UNDFINED = 0,
    NET_CMD_QUIT = 1,
    NET_CMD_HEARTBEAT = 2,
    NET_CMD_DATA = 3,
    NET_CMD_START = 4
};