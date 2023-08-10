#pragma once

enum
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
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

	// º¸³»±â
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

