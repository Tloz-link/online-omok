#pragma once

#include "CorePch.h"

using PacketRef = shared_ptr<class Packet>;

using GameManagerRef = shared_ptr<class GameManager>;
using GameSessionRef = shared_ptr<class GameSession>;
using GameRoomRef = std::shared_ptr<class GameRoom>;

using PlayerRef = std::shared_ptr<class Player>;

#include "Protocol.pb.h"
#include "Enum.pb.h"
#include "Struct.pb.h"

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "ServerPacketHandler.h"
