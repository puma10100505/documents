#include "GameObject.h"
#include "World.h"
#include "BufferWriter.h"
#include "gameplay.pb.h"

using namespace std;
using namespace yinpsoft;

void GameObject::Initialize(World* pw, float l, float w, float h, float d)
{
    set_world(pw);
    rigidbody_id = dBodyCreate(world_ptr()->GetPhyxWorld());
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
    mutable_position().set_x(pos.x());
    mutable_position().set_y(pos.y());    
    mutable_position().set_z(pos.z());
    dBodySetPosition(rigidbody_id, pos.x(), pos.y(), pos.z());
}

const Vector3& GameObject::GetPosition()
{
    // const dReal *tmp = dBodyGetPosition(rigidbody_id);
    // return Vector3(tmp[0], tmp[1], tmp[2]);

    return position();
}

void GameObject::SetRotation(const Quaternion &q)
{
    dQuaternion rotation;
    rotation[0] = q.x();
    rotation[1] = q.y();
    rotation[2] = q.z();
    rotation[3] = q.w();

    dBodySetQuaternion(rigidbody_id, rotation);
}

Quaternion GameObject::GetRotation()
{
    const dReal *tmp = dBodyGetQuaternion(rigidbody_id);
    return Quaternion(tmp[0], tmp[1], tmp[2], tmp[3]);
}

void GameObject::Replicate(BufferWriter& writer)
{
    Vector3 pos = GetPosition();
    pb::PBGameObject data;
    data.mutable_position()->set_x(pos.x());
    data.mutable_position()->set_y(pos.y());
    data.mutable_position()->set_z(pos.z());
    data.set_goid(goid());

    printf("before write pb, msg: %s\n", data.ShortDebugString().c_str());
    writer.WriteProto(data);
}