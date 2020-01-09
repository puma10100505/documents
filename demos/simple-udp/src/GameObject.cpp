#include "GameObject.h"
#include "World.h"

using namespace std;
using namespace yinpsoft;

GameObject::GameObject(World *w)
{
    set_world(w);
}

void GameObject::Initialize(float l, float w, float h, float d)
{
    dBodyCreate(world_ptr()->GetPhyxWorld());
    set_length(l);
    set_width(w);
    set_height(h);
    set_density(d);
    SetMass();

    // Create Geometry
    geometry_id = dCreateBox(world_ptr()->GetPhyxSpace(), length(), width(), height());
    dGeomSetBody(geometry_id, rigidbody_id);
}

void GameObject::SetMass()
{
    dMassSetZero(&mass);
    dMassSetBox(&mass, density(), length(), width(), height());
    dBodySetMass(rigidbody_id, &mass);
}

void GameObject::SetPosition(const Vector3 &pos)
{
    dBodySetPosition(rigidbody_id, pos.x, pos.y, pos.z);
}

Vector3 GameObject::GetPosition()
{
    const dReal *tmp = dBodyGetPosition(rigidbody_id);
    return Vector3(tmp[0], tmp[1], tmp[2]);
}

void GameObject::SetRotation(const Quaternion &q)
{
    dQuaternion rotation;
    rotation[0] = q.x;
    rotation[1] = q.y;
    rotation[2] = q.z;
    rotation[3] = q.w;

    dBodySetQuaternion(rigidbody_id, rotation);
}

Quaternion GameObject::GetRotation()
{
    const dReal *tmp = dBodyGetQuaternion(rigidbody_id);
    return Quaternion(tmp[0], tmp[1], tmp[2], tmp[3]);
}