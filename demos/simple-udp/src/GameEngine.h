#pragma once

#include "Header.h"
#include "PropertyMacros.h"

static const uint32_t server_appid = 0x11223344;
static const unsigned short server_port = 8888;

namespace yinpsoft
{
class GameEngine final
{
public:
    GameEngine() {}
    ~GameEngine() {}

    GameEngine &Initialize(int32_t interval = 30);

public:
    void Startup();
    void Shutdown();

private:
    void OnTick();

private:
    uint64_t server_tick;
    int32_t tick_interval_ms;
    GETSETVAR(bool, running, false);
};
}; // namespace yinpsoft