#pragma once

class GameManager
{
public:
	bool SignUp(GameSessionRef session, string name);

	void MatchRoom(GameSessionRef session);
	void LeaveRoom(GameSessionRef session);

private:
	//GameRoomRef CreateGameRoom();

private:
	set<GameRoomRef> _gameRooms;

private:
	static atomic<uint64> s_idGenerator;
};

extern GameManager GGameManager;

