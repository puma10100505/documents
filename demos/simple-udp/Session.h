#pragma once

#include "Header.h"
#include <unordered_map>
#include <vector>
#include "PropertyMacros.h"

namespace yinpsoft
{
    class RawPackage;

    class Session final 
    {
        public:
            Session() {}
            ~Session() {}

            void PushPendingRecv(const RawPackage& pkg);
            void PushPendingSend(const RawPackage& pkg);

            inline const std::vector<RawPackage>& PendingSendList() { return pending_send_list; }
            inline const std::vector<RawPackage>& PendingRecvList() { return pending_recv_list; }

            int32_t CommandDispatcher(uint8_t cmdid, const RawPackage& pkg);

        private:
            GETSETVAR(uint32_t, sid, 0);
            std::vector<RawPackage> pending_recv_list;
            std::vector<RawPackage> pending_send_list;
            GETSETARRAY(uint8_t, sending_buff, DEFAULT_MSS);
            GETSETVAR(size_t, send_len, 0);
            GETSETVAR(int64_t, guid, 0);
    };

    class SessionManager final 
    {
        public:
            SessionManager() {}
            ~SessionManager() {}

            void AddSession(uint32_t sid, const Session& sess);
            const Session* GetSession(uint32_t sid, bool force_new = true);
            bool IsSessionExist(uint32_t sid);
            void RemoveSession(uint32_t sid);
            Session* CreateSession();
            

        private:
            std::unordered_map<uint32_t, std::unique_ptr<class Session>> session_list;
            static int32_t sid_seq;
    };
};