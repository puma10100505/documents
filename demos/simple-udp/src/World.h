#pragma once

#include "PropertyMacros.h"

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

        private:
            GETSETVAR(uint64_t, battle_id, 0);

            std::unordered_map<uint32_t, GameObject*> gos;

            dWorldID phyx_world;
            dSpaceID phyx_space;
            dJointGroupID contact_group;
            //dsFunctions fn;
    };
};