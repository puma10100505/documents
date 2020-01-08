#pragma once

enum ENetCommandID : int
{
    NET_CMD_UNDFINED = 0,
    NET_CMD_QUIT = 1,
    NET_CMD_HEARTBEAT = 2,
    NET_CMD_DATA = 3,
    NET_CMD_START = 4,
    NET_CMD_BUILD_WORLD = 5,
    NET_CMD_FILL_PLAYER = 6,
    NET_CMD_MAX
};