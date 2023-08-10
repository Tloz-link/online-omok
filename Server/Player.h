#pragma once

class Player
{
public:
	Player();
	virtual ~Player();

public:
	Protocol::PlayerInfo playerInfo;

public:
	weak_ptr<GameSession> session;
	weak_ptr<GameRoom> gameRoom;
};

