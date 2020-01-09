#include "World.h"
#include "GameObject.h"
#include "ode/ode.h"
#include "boost/bind/bind.hpp"
#include "boost/function.hpp"

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

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            std::unique_ptr<GameObject> go(new GameObject(this));
            go->Initialize(2.0f, 2.0f, 2.0f, 1.0f);
            go->SetPosition(Vector3((i * 1.0f * 2.0f) - 16, (1.0f * 2.0f * j) - 16, 0.0f));
            gos[goid_generator()] = std::move(go);

            inc_goid_generator();
        }
    }
}

void World::CalcCollision(void *data, dGeomID o1, dGeomID o2)
{
    const int N = 10;
    dContact contact[N];

    //int isGround = ((o1 == ground) || (o2 == ground));
    int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));

    for (int i = 0; i < n; i++)
    {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.bounce = 0.2f;
        contact[i].surface.bounce_vel = 0.001f;
        dJointID c = dJointCreateContact(phyx_world, contact_group, &contact[i]);
        dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
    }
}

void World::Tick()
{
    dSpaceCollide(phyx_space, 0, (dNearCallback *)&boost::bind(&World::CalcCollision, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3));
    dWorldStep(phyx_world, 0.01);
    dJointGroupEmpty(contact_group);
}

void World::Destroy()
{
    dCloseODE();
}