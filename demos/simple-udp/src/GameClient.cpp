#include "GameClient.h"

void GameClient::Init()
{
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
}

void GameClient::Start()
{

}

void GameClient::OnUpdate()
{
    for (int32_t i = 0; i < static_cast<int32_t>(pending_recv_list.size()); i++)
    {
        const ReceivedPackage &pkg = pending_recv_list[i];

        switch (pkg.cmd)
        {
        case ENetCommandID::NET_CMD_START:
            ResolveStart(pkg.package.start);
            break;
        case ENetCommandID::NET_CMD_QUIT:
            ResolveQuit(pkg.package.quit);
            break;
        case ENetCommandID::NET_CMD_OBJECT_SPAWN:
            //TODO:
            ResolveGameObject(pkg.package.go);
            break;
        default:
            break;
        }
    }

    pending_recv_list.clear();
}

void GameClient::ResolveGameObject(const pb::PBGameObject& pkg)
{
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
