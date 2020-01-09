#pragma once

#include "PropertyMacros.h"
#include "Common.h"
#include "ode/ode.h"

namespace yinpsoft
{
    class Vector3;
    class Rotation;

    class GameObject final
    {
        public:
            GameObject(){}
            ~GameObject(){}

            void Initialize();

            inline const dBodyID& GetRigidbody() const { return rigidbody_id; }
            inline const dGeomID& GetGeometry() const { return geometry_id; }

        private:
            GETSETVAR(uint32_t, goid, 0);
            GETSETVAR(Vector3, position, Vector3::Zero());
            GETSETVAR(Rotation, rotation, Rotation::Identity());
            
            dBodyID rigidbody_id;   /// Rigidbody
            dGeomID geometry_id;    /// Geometry
    };
};