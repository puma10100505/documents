#pragma once

#include "Header.h"
#include <unordered_map>
#include <vector>
#include "PropertyMacros.h"
#include "NetMessage.h"
#include "NetAddress.h"

namespace yinpsoft
{
class RUDPServer;
class NetAddress;
class BufferReader;

class Session final
{
public:
    Session() {}
    ~Session() {}

    void PushPendingRecv(const RawPackage &pkg);
    void PushPendingSend(const RawPackage &pkg);

    inline const std::vector<RawPackage> &PendingSendList() { return pending_send_list; }
    inline const std::vector<RawPackage> &PendingRecvList() { return pending_recv_list; }

    int32_t CommandDispatcher(uint8_t cmdid, BufferReader &reader);
    void SendPackage(const BufferWriter &writer);
    inline void SetClientAddress(const NetAddress &addr) { client_addr = addr; }

private:
    void HandleQuit(const QuitReq &pkg);
    void HandleHeartbeat(const RawPackage &pkg);
    void HandleData(const RawPackage &pkg);
    void HandleStart(const StartReq &pkg);    

private:
    GETSETVAR(uint32_t, sid, 0);
    std::vector<RawPackage> pending_recv_list;
    std::vector<RawPackage> pending_send_list;
    GETSETARRAY(uint8_t, sending_buff, DEFAULT_MSS);
    GETSETVAR(size_t, send_len, 0);
    GETSETVAR(int64_t, guid, 0);
    GETSETPTR(class RUDPServer, server);
    NetAddress client_addr;
};

class SessionManager final
{
public:
    SessionManager() {}
    ~SessionManager() {}

    Session *GetSession(uint32_t sid);
    bool IsSessionExist(uint32_t sid);
    void RemoveSession(uint32_t sid);
    Session *CreateSession(RUDPServer *server);
    inline int32_t Count() { return static_cast<int32_t>(session_list.size()); }
    inline std::unordered_map<uint32_t, std::unique_ptr<class Session>>& AllSessions() { return session_list; }

private:
    std::unordered_map<uint32_t, std::unique_ptr<class Session>> session_list;
    static uint32_t sid_seq;
};
}; // namespace yinpsoft