#include "pch.h"
#include "PacketQueue.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"
#include "GameManager.h"
#include "Player.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(GetSessionRef());
	Send(ServerPacketHandler::Make_S_Login(false, Protocol::LOG_IN_ERROR_NO_ACCOUNT, -1, ""));
}

void GameSession::OnDisconnected()
{
	PacketRef packet = make_shared<Packet>(GetSessionRef(), nullptr, 0);
	GPacketQueue.Push(packet);
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketRef packet = make_shared<Packet>(GetSessionRef(), buffer, len);
	GPacketQueue.Push(packet);
}

void GameSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}