#pragma once

class GameRoom : public enable_shared_from_this<GameRoom>
{
public:
	GameRoom();
	virtual ~GameRoom();

	void Init();
	void Update();

	bool EnterRoom(PlayerRef player);
	void LeaveRoom(PlayerRef player);
	bool Empty();

	GameRoomRef GetRoomRef() { return shared_from_this(); }

public:
	void StartGame();
	void PlayStone(PlayerRef player, Protocol::C_PlayStone& stoneInfo);

private:
	bool CheckWin(const Protocol::StoneInfo& stoneInfo);
	uint16 CheckWinRecursive(const Protocol::StoneInfo& stoneInfo, uint16 offsetY, uint16 offsetX);

private:
	vector<Protocol::StoneInfo> _stones;
	vector<PlayerRef> _players;
	bool _start;

private:
	const uint16 MAX_LINE = 19;
};

extern GameRoomRef GRoom;