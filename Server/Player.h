#pragma once

class Player
{
public:
	Player();
	virtual ~Player()
	{
		cout << "Leave Player" << endl;
	}

public:
	Protocol::PlayerInfo playerInfo;

public:
	weak_ptr<GameSession> session;
	weak_ptr<GameRoom> gameRoom;
};

