#include "GameClient.h"
#include "GameObject.h"
#include "Common.h"

void GameClient::Initialize(uint32_t appid, unsigned int address,
                           unsigned short port, bool shm,
                           int32_t interval_ms)
{
    RUDPClient::Initialize(appid, address, port);
}

void GameClient::Start()
{
    if (status() == EClientStatus::CS_NONE)
    {
        PushCommandLine(ENetCommandID::NET_CMD_START);
    }

    RUDPClient::Run();
}

void GameClient::OnUpdate()
{
    printf("updating in gameclient, client_status: %d\n", status());
    switch (status()) 
    {
        case EClientStatus::CS_START:
            printf("entry of processing for start status");
            PushCommandLine(ENetCommandID::NET_CMD_PLAYER_ENTER);
            set_status(EClientStatus::CS_READY);
            break;

        default:
            break;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(pending_recv_list.size()); i++)
    {
        const ReceivedPackage &pkg = pending_recv_list[i];

        switch (pkg.cmd)
        {
        case ENetCommandID::NET_CMD_START:
            ResolveStart(pkg.package.start);
            set_status(EClientStatus::CS_START);
            break;
        case ENetCommandID::NET_CMD_QUIT:
            ResolveQuit(pkg.package.quit);
            set_status(EClientStatus::CS_QUIT);
            break;
        case ENetCommandID::NET_CMD_OBJECT_SPAWN:
            //TODO:
            ResolveGameObject(pkg.go);

            break;
        default:
            break;
        }
    }

    pending_recv_list.clear();
}

void GameClient::ResolveGameObject(const pb::PBGameObject& pkg)
{
    printf("ResolveGameObject entry of method, pkg: %s\n", pkg.ShortDebugString().c_str());
    if (gos.find(pkg.goid()) == gos.end())
    {
        std::unique_ptr<GameObject> go_ptr(new GameObject());
        GameObject* go = go_ptr.get();
        if (go == nullptr) 
        {
            printf("create gameobject in client failed\n");
            return;
        }

        go->set_goid(pkg.goid());
        go->SetPosition(Vector3(pkg.position().x(), pkg.position().y(), pkg.position().z()));
        

        gos[pkg.goid()] = std::move(go_ptr);
    }
}

void GameClient::ResolveQuit(const QuitResp &pkg)
{
    printf("entry of ResolveQuit.......... pkg: %s\n", pkg.ToString().c_str());

    set_sid(0);
}

void GameClient::ResolveStart(const StartResp &pkg)
{
    // TODO: 处理START回包
    printf("entry of ResolveStart.......... pkg: %s\n", pkg.ToString().c_str());

    set_sid(pkg.sid);
    set_battle_id(pkg.battle_id);
}
