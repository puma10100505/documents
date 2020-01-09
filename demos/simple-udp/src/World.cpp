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
            gos[goid_generator()] = std::move(go);

            inc_goid_generator();
        }
    }
}

void World::CalcCollision(void *data, dGeomID o1, dGeomID o2)
{
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
    dWorldStep(phyx_world, 0.01);
    dJointGroupEmpty(contact_group);
}

void World::Replicate()
{
    BufferWriter writer;

    // 1. Write message header
    NetHeader header;
    header.cmd = ENetCommandID::NET_CMD_OBJECT_REPLICATE;

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

    printf("all session count: %d\n", Singleton<SessionManager>::get_mutable_instance().Count());

    // 3. push the data to all the sessions
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

void World::Destroy()
{
    dCloseODE();
}