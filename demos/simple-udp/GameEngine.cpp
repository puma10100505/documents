#include "GameEngine.h"
#include "Singleton.h"
#include "RUDPServer.h"

#include <chrono>

using namespace yinpsoft;
using namespace std;

void GameEngine::Shutdown()
{
    Singleton<RUDPServer>::get_mutable_instance().Stop();
}

void GameEngine::Startup()
{
    chrono::milliseconds tick_interval(tick_interval_ms);

    while (true)
    {
        OnTick();

        server_tick++;
        this_thread::sleep_for(tick_interval);
    }
}


void GameEngine::OnTick()
{
    Singleton<RUDPServer>::get_mutable_instance().Tick();
}

GameEngine& GameEngine::Initialize(int32_t interval)
{
    tick_interval_ms = interval;

    Singleton<RUDPServer>::get_mutable_instance().Initialize(server_appid, server_port);

    return *this;
}