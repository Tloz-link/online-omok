#pragma once

class Packet
{
public:
	Packet(GameSessionRef session, BYTE* buffer, int32 len);
	~Packet();

public:
	GameSessionRef GetSession() { return _session; }
	BYTE* GetBuffer() { return _buffer; }
	int32 GetLen() { return _len; }

private:
	GameSessionRef _session;
	BYTE* _buffer;
	int32 _len;
};

class PacketQueue
{
public:
	PacketQueue();
	~PacketQueue();

public:
	void Push(PacketRef);
	PacketRef Pop();

private:
	USE_LOCK;
	queue<PacketRef> _packetQueue;
};

extern PacketQueue GPacketQueue;