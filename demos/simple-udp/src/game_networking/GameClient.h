#pragma once

#include "RUDPClient.h"
#include "NetMessage.h"
#include "gameplay.pb.h"

namespace yinpsoft
{
    class RUDPClient;
    class GameObject;

    class GameClient final : public RUDPClient
    {
        public:
            GameClient() {}
            ~GameClient(){}

        public:
            virtual void Initialize(uint32_t appid, unsigned int address,
                           unsigned short port, bool shm = false,
                           int32_t interval_ms = 100) override;
            void Start();
            
            virtual void OnUpdate() override;

            void ResolveStart(const StartResp &pkg);
            void ResolveQuit(const QuitResp &pkg);
            void ResolveGameObject(const pb::PBGameObject& pkg);

            inline const std::unordered_map<uint32_t, std::unique_ptr<GameObject>>& AllGameObject() { return gos; }

        private:
            std::unordered_map<uint32_t, std::unique_ptr<GameObject>> gos;
            
    };
};