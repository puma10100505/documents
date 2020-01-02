#include "Session.h"
#include "NetMessage.h"
#include "Commands.h"

using namespace std;
using namespace yinpsoft;

void Session::PushPendingRecv(const RawPackage &pkg)
{
    pending_send_list.emplace_back(pkg);
}

void Session::PushPendingSend(const RawPackage &pkg)
{
    pending_recv_list.emplace_back(pkg);
}

int32_t Session::CommandDispatcher(uint8_t cmdid, const RawPackage &pkg)
{
    switch (cmdid)
    {
    case ENetCommandID::NET_CMD_START:
    {
        break;
    }

    case ENetCommandID::NET_CMD_QUIT:
    {
        break;
    }

    case ENetCommandID::NET_CMD_HEARTBEAT:
    {
        break;
    }

    case ENetCommandID::NET_CMD_DATA:
    {
        // Data package, directly store into session(channel)
        break;
    }
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////
uint32_t SessionManager::sid_seq = 0;

Session *SessionManager::GetSession(uint32_t sid)
{
    if (IsSessionExist(sid))
    {
        return session_list[sid].get();
    }

    return nullptr;
}

bool SessionManager::IsSessionExist(uint32_t sid)
{
    return session_list.find(sid) != session_list.end();
}

void SessionManager::RemoveSession(uint32_t sid)
{
    if (IsSessionExist(sid))
    {
        session_list.erase(sid);
    }
}

Session *SessionManager::CreateSession(int64_t guid)
{
    // TODO: Remove old session with the same guid

    SessionManager::sid_seq++;

    std::unique_ptr<Session> new_s(new Session());
    new_s->set_sid(SessionManager::sid_seq);
    new_s->set_guid(guid);

    session_list[SessionManager::sid_seq] = std::move(new_s);

    return new_s.get();
}