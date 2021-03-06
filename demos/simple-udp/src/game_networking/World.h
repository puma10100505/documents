#pragma once

#include "PropertyMacros.h"
#include "ode/ode.h"
#include "GameObject.h"
#include <memory>

namespace yinpsoft
{
typedef struct 
{
    dWorldID phyx_world;
    dJointGroupID contact_group;
} CollisionParams;

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
    void Replicate();

    inline dWorldID &GetPhyxWorld() { return phyx_world; }
    inline dSpaceID &GetPhyxSpace() { return phyx_space; }

    void SpawnObject(GameObject* go, uint32_t sid = 0);
    void ReplicateAllGameObjects(uint32_t sid);

private:
    static void CalcCollision(void *data, dGeomID o1, dGeomID o2);
    void SendToAllClient(BufferWriter& writer);
    void SendBySessionID(BufferWriter& writer, uint32_t sid);

private:
    GETSETVAR(uint64_t, battle_id, 0);
    INCVAR(uint32_t, goid_generator);

    std::unordered_map<uint32_t, std::unique_ptr<GameObject>> gos;

    dWorldID phyx_world;
    dSpaceID phyx_space;
    dJointGroupID contact_group;

    dGeomID ground;

    //dsFunctions fn;
};
}; // namespace yinpsoft