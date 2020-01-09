#pragma once

#include "PropertyMacros.h"
#include "ode/ode.h"

namespace yinpsoft
{
class GameObject;

class World final
{
public:
    World() {}
    ~World() {}

public:
    void Initialize();
    void Tick();
    void Destroy();

    inline dWorldID &GetPhyxWorld() { return phyx_world; }
    inline dSpaceID &GetPhyxSpace() { return phyx_space; }

private:
    void CalcCollision(void *data, dGeomID o1, dGeomID o2);

private:
    GETSETVAR(uint64_t, battle_id, 0);
    INCVAR(uint32_t, goid_generator, 0);

    std::unordered_map<uint32_t, std::unique_ptr<GameObject>> gos;

    dWorldID phyx_world;
    dSpaceID phyx_space;
    dJointGroupID contact_group;

    dGeomID ground;

    //dsFunctions fn;
};
}; // namespace yinpsoft