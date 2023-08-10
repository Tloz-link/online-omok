#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "GameManager.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "GameRoom.h"
#include "Player.h"


void ServerPacketHandler::HandlePacket(GameSessionRef session, BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{
	case C_SignUp:
		Handle_C_SignUp(session, buffer, len);
		break;
	case C_EnterRoom:
		Handle_C_EnterRoom(session, buffer, len);
		break;
	case C_PlayStone:
		Handle_C_PlayStone(session, buffer, len);
		break;
	}
}

void ServerPacketHandler::Handle_C_SignUp(GameSessionRef session, BYTE* buffer, int32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;
	uint16 size = header->size;

	Protocol::C_SignUp pkt;
	pkt.ParseFromArray(&header[1], size - sizeof(PacketHeader));

	string name = pkt.name();

	GGameManager.SignUp(session, name);
}

void ServerPacketHandler::Handle_C_EnterRoom(GameSessionRef session, BYTE* buffer, int32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;
	uint16 size = header->size;

	Protocol::C_EnterRoom pkt;
	pkt.ParseFromArray(&header[1], size - sizeof(PacketHeader));

	GGameManager.MatchRoom(session);
}

void ServerPacketHandler::Handle_C_PlayStone(GameSessionRef session, BYTE* buffer, int32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;
	uint16 size = header->size;

	Protocol::C_PlayStone pkt;
	pkt.ParseFromArray(&header[1], size - sizeof(PacketHeader));

	PlayerRef player = session->player;
	GameRoomRef room = player->gameRoom.lock();

	room->PlayStone(player, pkt);
}

SendBufferRef ServerPacketHandler::Make_S_Login(bool success, Protocol::LOG_IN_ERROR error, uint64 accountId, string name)
{
	Protocol::S_Login pkt;

	pkt.set_success(success);
	pkt.set_error(error);

	Protocol::PlayerInfo* playerInfo = pkt.mutable_playerinfo();
	playerInfo->set_playerid(accountId);
	playerInfo->set_name(name);
	if (success)
		playerInfo->set_playerstate(Protocol::PLAYER_STATE_CONNECTED);
	else
		playerInfo->set_playerstate(Protocol::PLAYER_STATE_DISCONNECTED);
	playerInfo->set_playertype(Protocol::PLAYER_TYPE_NONE);

	return MakeSendBuffer(pkt, S_Login);
}

SendBufferRef ServerPacketHandler::Make_S_ThrowSignUpError(Protocol::SIGN_UP_ERROR error)
{
	Protocol::S_ThrowSignUpError pkt;

	pkt.set_error(error);

	return MakeSendBuffer(pkt, S_ThrowSignUpError);
}

SendBufferRef ServerPacketHandler::Make_S_StartGame(string name, Protocol::PLAYER_TYPE type)
{
	Protocol::S_StartGame pkt;

	Protocol::EnemyInfo* enemyInfo = pkt.mutable_enemyinfo();
	enemyInfo->set_name(name);
	enemyInfo->set_playertype(type);

	return MakeSendBuffer(pkt, S_StartGame);
}

SendBufferRef ServerPacketHandler::Make_S_LeaveGame(bool success)
{
	Protocol::S_LeaveGame pkt;

	pkt.set_success(success);

	return MakeSendBuffer(pkt, S_LeaveGame);
}

SendBufferRef ServerPacketHandler::Make_S_CheckStone(bool enable)
{
	Protocol::S_CheckStone pkt;

	pkt.set_enable(enable);

	return MakeSendBuffer(pkt, S_CheckStone);
}

SendBufferRef ServerPacketHandler::Make_S_PlayStone(Protocol::StoneInfo stoneInfo)
{
	Protocol::S_PlayStone pkt;

	Protocol::StoneInfo* info = pkt.mutable_stoneinfo();
	*info = stoneInfo;

	return MakeSendBuffer(pkt, S_PlayStone);
}

SendBufferRef ServerPacketHandler::Make_S_ThrowResult(Protocol::StoneInfo stoneInfo)
{
	Protocol::S_ThrowResult pkt;

	Protocol::StoneInfo* info = pkt.mutable_stoneinfo();
	*info = stoneInfo;

	return MakeSendBuffer(pkt, S_ThrowResult);
}