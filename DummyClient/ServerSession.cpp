#include "pch.h"
#include "ServerSession.h"
#include "ClientPacketHandler.h"

ServerSession::ServerSession()
{

}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	ClientPacketHandler::HandlePacket(GetSessionRef(), buffer, len);
}