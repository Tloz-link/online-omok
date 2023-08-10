#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

/*--------------
	Listener
---------------*/

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (IocpEvent* acceptEvent : _acceptEvents)
	{
		// TODO
		delete acceptEvent;
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
	{
		cout << "_service null" << endl;
		return false;
	}

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
	{
		cout << "Failed CreateSocket()" << endl;
		return false;
	}

	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
	{
		cout << "Failed SetReuseAddress()" << endl;
		return false;
	}

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
	{
		cout << "Failed SetLinger()" << endl;
		return false;
	}

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
	{
		cout << "Failed Bind()" << endl;
		return false;
	}

	if (SocketUtils::Listen(_socket) == false)
	{
		cout << "Failed Listen()" << endl;
		return false;
	}

	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		IocpEvent* acceptEvent = new IocpEvent(EventType::Accept);
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* acceptEvent, int32 numOfBytes)
{
	assert(acceptEvent->type == EventType::Accept);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(IocpEvent* acceptEvent)
{
	SessionRef session = _service->CreateSession(); // Register IOCP

	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT &bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(IocpEvent* acceptEvent)
{
	SessionRef session = acceptEvent->session;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&sockAddress;
	struct in_addr ipAddr = pV4Addr->sin_addr;
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
	cout << "Client Connected! IP = " << str << endl;

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}