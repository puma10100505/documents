/*
TODO: 1.梳理并重新设置网络层及应用层协议包结构
*/
#include "Header.h"

#include "NetSocket.h"
#include "Singleton.h"
#include "RUDPClient.h"
#include "RUDPServer.h"
#include "BufferWriter.h"
#include "BufferReader.h"
#include "Commands.h"
#include "GameEngine.h"
#include <chrono>
#include <memory>
#include "NetMessage.h"
#include "google/protobuf/message.h"

#ifdef GLCLI
#include "GameRenderer.h"
#endif

using namespace yinpsoft;

static std::string app_type;

void interrupt_signal_handler(int signum)
{
    printf("Ctrl+C interrupted...\n");

    if (app_type == "cli" || app_type == "client" || app_type == "rawcli")
    {
        Singleton<RUDPClient>::get_mutable_instance().set_running(false);
        exit(0);
    }

    if (app_type == "svr" || app_type == "server")
    {
        Singleton<GameEngine>::get_mutable_instance().set_running(false);
        exit(0);
    }
}

int main(int argc, char **argv)
{
    signal(SIGINT, interrupt_signal_handler);

    if (argc < 2)
    {
        printf("the paramter num is illegal\n");
        return -1;
    }

    app_type = argv[1];

    if (app_type == "svr" || app_type == "server")
    {
        printf("start server ...\n");
        Singleton<GameEngine>::get_mutable_instance().Initialize().Startup();
    }
    else if (app_type == "rawcli")
    {
        printf("start client ...\n");
        Singleton<RUDPClient>::get_mutable_instance().Initialize(0x11223344, htonl(inet_addr("127.0.0.1")), 8888);
        Singleton<RUDPClient>::get_mutable_instance().Run();
    }
#ifdef GLCLI
    else if (app_type == "cli" || app_type == "client")
    {
        printf("start client ...\n");
        //InitWorld(argc, argv);
        GameStart(argc, argv);
    }
#else
    else if (app_type == "cli" || app_type == "client")
    {
        printf("start raw client ...\n");
        Singleton<RUDPClient>::get_mutable_instance().Initialize(0x11223344, htonl(inet_addr("127.0.0.1")), 8888);
        Singleton<RUDPClient>::get_mutable_instance().Run();
    }
#endif

    return 0;
}