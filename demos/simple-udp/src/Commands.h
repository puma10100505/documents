#pragma once

enum ENetCommandID : int
{
    NET_CMD_UNDFINED = 0,
    NET_CMD_QUIT = 1,
    NET_CMD_HEARTBEAT = 2,
    NET_CMD_DATA = 3,
    NET_CMD_START = 4,
    NET_CMD_BUILD_WORLD = 5,
    NET_CMD_PLAYER_ENTER = 6,
    NET_CMD_OBJECT_SPAWN = 7,           // Create gameobject
    NET_CMD_OBJECT_REPLICATE = 8,       // Sync state of gameobject
    NET_CMD_MAX
};