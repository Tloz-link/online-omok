#pragma once

enum PacketID
{
	S_Login = 1,

	C_SignUp = 2,
	S_ThrowSignUpError = 3,

	C_EnterRoom = 4,
	S_StartGame = 5,
	S_LeaveGame = 6,

	C_PlayStone = 7,
	S_PlayStone = 8,

	S_CheckStone = 9,
	S_ThrowResult = 10,

	C_Dummy = 11,
};

class ServerPacketHandler
{
public:
	static void HandlePacket(GameSessionRef session, BYTE* buffer, int32 len);

	// 받기
	static void Handle_C_SignUp(GameSessionRef session, BYTE* buffer, int32 len);
	static void Handle_C_EnterRoom(GameSessionRef session, BYTE* buffer, int32 len);
	static void Handle_C_PlayStone(GameSessionRef session, BYTE* buffer, int32 len);
	static void Handle_C_Dummy(GameSessionRef session, BYTE* buffer, int32 len);
	
	// 보내기
	static SendBufferRef Make_S_Login(bool success, Protocol::LOG_IN_ERROR error, uint64 accountId, string name);
	static SendBufferRef Make_S_ThrowSignUpError(Protocol::SIGN_UP_ERROR error);
	static SendBufferRef Make_S_StartGame(string name, Protocol::PLAYER_TYPE type);
	static SendBufferRef Make_S_LeaveGame(bool success);
	static SendBufferRef Make_S_CheckStone(bool enable);
	static SendBufferRef Make_S_PlayStone(Protocol::StoneInfo stoneInfo);
	static SendBufferRef Make_S_ThrowResult(Protocol::StoneInfo stoneInfo);

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};