#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "ServerSession.h"


void ClientPacketHandler::HandlePacket(ServerSessionRef session, BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
		case S_Login:
			Handle_S_Login(session, buffer, len);
			break;
		case S_StartGame:
			Handle_S_StartGame(session, buffer, len);
			break;
	}
}

void ClientPacketHandler::Handle_S_Login(ServerSessionRef session, BYTE* buffer, int32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;
	uint16 size = header->size;

	Protocol::S_Login pkt;
	pkt.ParseFromArray(&header[1], size - sizeof(PacketHeader));

	if (pkt.success() == false)
	{
		session->Send(ClientPacketHandler::Make_C_SignUp(""));
	}
	else
	{
		cout << pkt.playerinfo().playerid() << " player access!" << endl;
		session->Send(ClientPacketHandler::Make_C_EnterRoom(pkt.playerinfo().playerid()));
	}
}

void ClientPacketHandler::Handle_S_StartGame(ServerSessionRef session, BYTE* buffer, int32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;
	uint16 size = header->size;

	Protocol::S_StartGame pkt;
	pkt.ParseFromArray(&header[1], size - sizeof(PacketHeader));

	this_thread::sleep_for(1s);

	session->Send(ClientPacketHandler::Make_C_Dummy(true));
}

SendBufferRef ClientPacketHandler::Make_C_SignUp(string name)
{
	Protocol::C_SignUp pkt;
	
	pkt.set_name(name);

	return MakeSendBuffer(pkt, C_SignUp);
}

SendBufferRef ClientPacketHandler::Make_C_EnterRoom(uint64 id)
{
	Protocol::C_EnterRoom pkt;

	pkt.set_accountid(id);

	return MakeSendBuffer(pkt, C_EnterRoom);
}

SendBufferRef ClientPacketHandler::Make_C_Dummy(bool success)
{
	Protocol::C_Dummy pkt;

	pkt.set_success(success);

	return MakeSendBuffer(pkt, C_Dummy);
}