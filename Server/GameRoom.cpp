#include "pch.h"
#include "GameRoom.h"
#include "GameSession.h"
#include "Player.h"
#include "ServerPacketHandler.h"

GameRoomRef GRoom = make_shared<GameRoom>();

GameRoom::GameRoom() : _start(false)
{

}

GameRoom::~GameRoom()
{

}

void GameRoom::Init()
{

}

void GameRoom::Update()
{

}

bool GameRoom::EnterRoom(PlayerRef player)
{
	if (_players.size() >= 2)
	{
		return false;
	}

	player->gameRoom = GetRoomRef();
	_players.push_back(player);
	if (_players.size() == 2)
	{
		StartGame();
	}

	return true;
}

void GameRoom::LeaveRoom(PlayerRef player)
{
	_players.erase(remove(_players.begin(), _players.end(), player), _players.end());
	for (PlayerRef player : _players)
	{
		player->session.lock()->Send(ServerPacketHandler::Make_S_LeaveGame(true));
	}
	_stones.clear();
	_start = false;
}

bool GameRoom::Empty()
{
	if (_players.empty() == true)
		return true;
	return false;
}

void GameRoom::StartGame()
{
	PlayerRef black = _players[0];
	PlayerRef white = _players[1];

	_stones.clear();
	for (int y = 0; y < MAX_LINE; ++y)
	{
		for (int x = 0; x < MAX_LINE; ++x)
		{
			Protocol::StoneInfo stone;
			stone.set_stonetype(Protocol::STONE_TYPE_BLANK);
			stone.set_posy(y);
			stone.set_posx(x);
			_stones.push_back(stone);
		}
	}

	black->playerInfo.set_playertype(Protocol::PLAYER_TYPE_BLACK);
	black->session.lock()->Send(ServerPacketHandler::Make_S_StartGame(white->playerInfo.name(), Protocol::PLAYER_TYPE_WHITE));

	white->playerInfo.set_playertype(Protocol::PLAYER_TYPE_WHITE);
	white->session.lock()->Send(ServerPacketHandler::Make_S_StartGame(black->playerInfo.name(), Protocol::PLAYER_TYPE_BLACK));

	_start = true;
}

void GameRoom::PlayStone(PlayerRef player, Protocol::C_PlayStone& pkt)
{
	GameSessionRef session = player->session.lock();
	if (_start == false)
	{
		session->Send(ServerPacketHandler::Make_S_CheckStone(false));
		return;
	}

	if (pkt.stoneinfo().posx() < 0 || pkt.stoneinfo().posx() >= MAX_LINE || pkt.stoneinfo().posy() < 0 || pkt.stoneinfo().posy() >= MAX_LINE)
	{
		session->Send(ServerPacketHandler::Make_S_CheckStone(false));
		return;
	}

	Protocol::StoneInfo& serverStone = _stones[pkt.stoneinfo().posy() * MAX_LINE + pkt.stoneinfo().posx()];
	if (serverStone.stonetype() != Protocol::STONE_TYPE_BLANK)
	{
		session->Send(ServerPacketHandler::Make_S_CheckStone(false));
		return;
	}

	// TODO 금지수 체크 해야함

	// 착수
	//session->Send(ServerPacketHandler::Make_S_CheckStone(true));
	serverStone.set_stonetype(pkt.stoneinfo().stonetype());

	SendBufferRef buffer = nullptr;
	if (CheckWin(serverStone) == true)
	{
		_start = false;
		buffer = ServerPacketHandler::Make_S_ThrowResult(pkt.stoneinfo());
	}
	else
	{
		buffer = ServerPacketHandler::Make_S_PlayStone(pkt.stoneinfo());
	}

	for (PlayerRef player : _players)
	{
		GameSessionRef s = player->session.lock();
		s->Send(buffer);
	}
}

bool GameRoom::CheckWin(const Protocol::StoneInfo& stoneInfo)
{
	// 가로
	if (CheckWinRecursive(stoneInfo, 0, 1) + CheckWinRecursive(stoneInfo, 0, -1) == 4)
	{
		return true;
	}

	// 세로
	if (CheckWinRecursive(stoneInfo, 1, 0) + CheckWinRecursive(stoneInfo, -1, 0) == 4)
	{
		return true;
	}

	// 오른쪽 아래 대각선
	if (CheckWinRecursive(stoneInfo, 1, 1) + CheckWinRecursive(stoneInfo, -1, -1) == 4)
	{
		return true;
	}

	// 오른쪽 위 대각선
	if (CheckWinRecursive(stoneInfo, -1, 1) + CheckWinRecursive(stoneInfo, 1, -1) == 4)
	{
		return true;
	}

	return false;
}

uint16 GameRoom::CheckWinRecursive(const Protocol::StoneInfo& stoneInfo, uint16 offsetY, uint16 offsetX)
{
	uint16 nextY = stoneInfo.posy() + offsetY;
	uint16 nextX = stoneInfo.posx() + offsetX;

	if (nextY < 0 || nextY >= MAX_LINE || nextX < 0 || nextX >= MAX_LINE)
	{
		return 0;
	}

	Protocol::StoneInfo& nextStone = _stones[nextY * MAX_LINE + nextX];
	if (stoneInfo.stonetype() != nextStone.stonetype())
	{
		return 0;
	}

	return 1 + CheckWinRecursive(nextStone, offsetY, offsetX);
}