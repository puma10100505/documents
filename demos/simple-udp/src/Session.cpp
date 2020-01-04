#include "Session.h"
#include "NetMessage.h"
#include "Commands.h"
#include "RUDPServer.h"
#include "NetAddress.h"

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
        HandleStartMessage(pkg);
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

void Session::HandleQuitMessage(const RawPackage &pkg)
{
    printf("handle quit\n");
}

void Session::HandleHeartbeatMessage(const RawPackage &pkg)
{
    printf("handle heartbeat\n");
}

void Session::HandleDataMessage(const RawPackage &pkg)
{
    printf("handle data\n");
}

void Session::HandleStartMessage(const RawPackage &pkg)
{
    printf("handle start\n");

    NetMessageHeader header;
    header.cmdid = ENetCommandID::NET_CMD_START;
    header.appid = appid;

    StartResponse resp;
    resp.guid = pkg.guid;
    resp.sid = sid();

    BufferWriter writer;
    header.Serialize(writer);
    resp.Serialize(writer);

    SendPackage(writer);
}

void Session::SendPackage(const BufferWriter &writer)
{
    if (server_ptr() == nullptr)
    {
        printf("server object is null\n");
        return;
    }

    printf("before send to client, len: %lu, address: %s, port: %u\n",
           writer.Length(), client_addr.ToString().c_str(), client_addr.GetPort());

    ssize_t ret = server_ptr()->GetServerSocket().SendTo(client_addr, writer.InternalBuffer(), writer.Length());

    printf("after send to client, ret: %ld\n", ret);

    return;
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

Session *SessionManager::CreateSession(int64_t guid, RUDPServer *server)
{
    // TODO: Remove old session with the same guid

    SessionManager::sid_seq++;

    std::unique_ptr<Session> new_s(new Session());
    new_s->set_sid(SessionManager::sid_seq);
    new_s->set_guid(guid);
    new_s->set_server(server);

    session_list[SessionManager::sid_seq] = std::move(new_s);

    return session_list[SessionManager::sid_seq].get();
}