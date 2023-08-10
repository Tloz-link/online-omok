#include "pch.h"
#include "PacketQueue.h"

PacketQueue GPacketQueue;

Packet::Packet(GameSessionRef session, BYTE* buffer, int32 len)
	: _session(session), _len(len)
{
	_buffer = nullptr;
	if (_len == 0)
	{
		return;
	}

	_buffer = new BYTE[len];
	for (int i = 0; i < len; ++i)
	{
		_buffer[i] = buffer[i];
	}
}

Packet::~Packet()
{
	if (_len == 0)
	{
		return;
	}

	delete[] _buffer;
}

PacketQueue::PacketQueue()
{

}

PacketQueue::~PacketQueue()
{

}

void PacketQueue::Push(PacketRef packet)
{
	WRITE_LOCK;
	_packetQueue.push(packet);
}

PacketRef PacketQueue::Pop()
{
	WRITE_LOCK;
	if (_packetQueue.size() == 0)
	{
		return nullptr;
	}

	PacketRef packet = _packetQueue.front();
	_packetQueue.pop();
	return packet;
}