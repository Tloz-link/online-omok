#include "pch.h"
#include "GameManager.h"
#include "GameSession.h"
#include "NetAddress.h"
#include "Player.h"
#include "ServerPacketHandler.h"
#include "GameRoom.h"

GameManager GGameManager;
atomic<uint64> GameManager::s_idGenerator = 1;

bool GameManager::SignUp(GameSessionRef session, string name)
{
	PlayerRef player = make_shared<Player>();
	player->playerInfo.set_playerid(s_idGenerator++);
	player->playerInfo.set_playertype(Protocol::PLAYER_TYPE_NONE);
	player->playerInfo.set_playerstate(Protocol::PLAYER_STATE_CONNECTED);
	player->playerInfo.set_name(name);

	player->session = session;
	session->player = player;

	session->Send(ServerPacketHandler::Make_S_Login(true, Protocol::LOG_IN_ERROR_NONE, player->playerInfo.playerid(), player->playerInfo.name()));
	return true;
}

void GameManager::MatchRoom(GameSessionRef session)
{
	for (GameRoomRef room : _gameRooms)
	{
		if (room->EnterRoom(session->player) == true)
		{
			return;
		}
	}

	GameRoomRef room = make_shared<GameRoom>();
	room->EnterRoom(session->player);
	_gameRooms.insert(room);
}

void GameManager::LeaveRoom(GameSessionRef session)
{
	PlayerRef player = session->player;
	if (player == nullptr)
	{
		return;
	}

	GameRoomRef room = player->gameRoom.lock();
	if (room == nullptr)
	{
		return;
	}

	room->LeaveRoom(player);
	if (room->Empty() == true)
	{
		_gameRooms.erase(room);
	}
}