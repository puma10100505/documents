#include "World.h"
#include "GameObject.h"
#include "ode/ode.h"
#include "boost/bind/bind.hpp"
#include "boost/function.hpp"
#include "gameplay.pb.h"
#include "BufferWriter.h"
#include "Commands.h"
#include "NetMessage.h"
#include "Session.h"
#include "Singleton.h"

using namespace std;
using namespace yinpsoft;

void World::Initialize()
{
    dInitODE();
    phyx_world = dWorldCreate();
    phyx_space = dHashSpaceCreate(0);
    contact_group = dJointGroupCreate(0);

    dWorldSetERP(phyx_world, 0.2f);
    dWorldSetCFM(phyx_world, 0.0005f);

    dWorldSetGravity(phyx_world, 0, 0, -20.0f);

    ground = dCreatePlane(phyx_space, 0, 0, 1, 0);

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            std::unique_ptr<GameObject> go(new GameObject());
            go->Initialize(this, 2.0f, 2.0f, 2.0f, 1.0f);
            go->SetPosition(Vector3((i * 1.0f * 2.0f) - 16, (1.0f * 2.0f * j) - 16, 0.0f));
            go->set_goid(goid_generator());

            SpawnObject(go.get());
            gos[goid_generator()] = std::move(go);            
            inc_goid_generator();
        }
    }

    printf("after world init\n");
}

void World::SpawnObject(GameObject* go)
{
    if (go == nullptr)
    {
        printf("game object is null\n");
        return;
    }

    BufferWriter writer;

    NetHeader header;
    header.cmd = ENetCommandID::NET_CMD_OBJECT_SPAWN;
    header.Serialize(writer);

    pb::PBGameObject obj;
    obj.mutable_position()->set_x(go->GetPosition().x());
    obj.mutable_position()->set_y(go->GetPosition().y());
    obj.mutable_position()->set_z(go->GetPosition().z());
    obj.set_goid(go->goid());

    writer.WriteProto(obj);

    printf("after write single gameobject, len: %lu, pos: %lu\n", writer.Length(), writer.Raw().Position());
    DumpPacket((char*)(writer.Raw().Buffer()), writer.Length());

    SendToAllClient(writer);
}

void World::SendToAllClient(BufferWriter& writer)
{
    printf("all session count: %d\n", Singleton<SessionManager>::get_mutable_instance().Count());
    if (Singleton<SessionManager>::get_mutable_instance().Count() == 0) 
    {
        return;
    }

    auto& all_sessions = Singleton<SessionManager>::get_mutable_instance().AllSessions();    
    for (auto& session_item: all_sessions)
    {
        uint32_t sid = session_item.first;
        Session* session = session_item.second.get();
        if (session == nullptr)
        {
            continue;
        }

        session->SendPackage(writer);
    }
}

void World::CalcCollision(void *data, dGeomID o1, dGeomID o2)
{    
    if (!o1 || !o2) 
    {
        return;
    }
    
    CollisionParams* params = (CollisionParams*)data;

    const int N = 10;
    dContact contact[N];

    int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));

    for (int i = 0; i < n; i++)
    {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.bounce = 0.2f;
        contact[i].surface.bounce_vel = 0.001f;

        dJointID c = dJointCreateContact(params->phyx_world, params->contact_group, &contact[i]);
        dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
    }
}

void World::Tick()
{
    CollisionParams params;
    params.contact_group = contact_group;
    params.phyx_world = phyx_world;
    dSpaceCollide(phyx_space, (void*)&params, &World::CalcCollision);
    contact_group = params.contact_group;

    // Phyx simulation
    dWorldStep(phyx_world, 0.015);

    dJointGroupEmpty(contact_group);

    Replicate();
}

void World::Replicate()
{
    BufferWriter writer;

    // 1. Write message header
    NetHeader header;
    header.cmd = ENetCommandID::NET_CMD_OBJECT_REPLICATE;
    header.Serialize(writer);

    // Sync gameobject data
    // 2. pack all the gameobject data into packet
    for (auto& goitem: gos)
    {
        GameObject* go = goitem.second.get();
        if (go == nullptr)
        {
            continue;
        }

        go->Replicate(writer);
    }

    printf("before send to all client: writer.len: %lu, writer.pos: %lu\n", writer.Length(), writer.Raw().Position());    

    // 3. push the data to all the sessions
    SendToAllClient(writer);
}

void World::Destroy()
{
    dCloseODE();
}