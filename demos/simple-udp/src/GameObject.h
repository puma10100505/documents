#pragma once

#include "PropertyMacros.h"
#include "Common.h"
#include "ode/ode.h"

namespace yinpsoft
{
class Vector3;
class Rotation;
class World;
class BufferWriter;

class GameObject final
{
public:
    GameObject() {}
    ~GameObject() {}

    void Initialize(World* pw, float l, float w, float h, float d);

    inline const dBodyID &GetRigidbody() const { return rigidbody_id; }
    inline const dGeomID &GetGeometry() const { return geometry_id; }

    inline const World &GetWorld() const
    {
        return world();
    }

    inline World *GetWorld_Mutable() { return world_ptr(); }

    void SetPosition(const Vector3 &pos);
    const Vector3& GetPosition();
    void SetRotation(const Quaternion &q);
    Quaternion GetRotation();

    void Replicate(BufferWriter& writer);

private:
    void SetMass();

private:
    GETSETVAR(uint32_t, goid, 0);
    GETSETVAR(Vector3, position, Vector3::Zero());
    // GETSETVAR(Rotation, rotation, Rotation::Identity());
    GETSETPTR(class World, world);

    dBodyID rigidbody_id; /// Rigidbody
    dGeomID geometry_id;  /// Geometry
    dMass mass;

    GETSETVAR(float, length, 1.0f);
    GETSETVAR(float, width, 1.0f);
    GETSETVAR(float, height, 1.0f);
    GETSETVAR(float, density, 1.0f);
};
}; // namespace yinpsoft