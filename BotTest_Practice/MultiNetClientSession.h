#pragma once
#include "Ringbuffer.h"
#include "LockFreeQueue.h"
#include "NetServerSerializeBuffer.h"

#define ONE_SEND_WSABUF_MAX					200

using MultiNetSessionId = UINT64;
class MultiNetClient;

class MultiNetClientSession
{
	friend MultiNetClient;

public:
	FORCEINLINE MultiNetSessionId GetSessionId() { return sessionId; }

private:
	struct OverlappedIOBase
	{
		WORD bufferCount;
		std::atomic_bool ioMode;
		OVERLAPPED overlapped;
	};

	struct OverlappedIORecvItem : public OverlappedIOBase
	{
		CRingbuffer ringBuffer;
	};

	struct OverlappedIOSendItem : public OverlappedIOBase
	{
		CLockFreeQueue<NetBuffer*> sendQ;
	};

private:
	SOCKET socket;
	std::atomic_bool isConnected = false;
	std::atomic_bool ioCancle = false;
	std::atomic_uint ioCount = 0;

	UINT nowPostQueuing = 0;

private:
	MultiNetSessionId sessionId = -1;
	
	std::atomic_bool sendDisconnect = false;
	std::atomic_bool isUseSession = false;

private:
	OverlappedIORecvItem recvIOItem;
	OverlappedIOSendItem sendIOItem;

	OVERLAPPED postQueueOverlapped;

private:
	NetBuffer* sendBufferStore[ONE_SEND_WSABUF_MAX];
};