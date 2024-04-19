#pragma once
#include "Ringbuffer.h"
#include "LockFreeStack.h"
#include "LockFreeQueue.h"
#include "ServerCommon.h"
#include "NetServerSerializeBuffer.h"
#include "MultiNetClientSession.h"

#define NONSENDING	0
#define SENDING		1

#define POST_RETVAL_ERR_SESSION_DELETED		0
#define POST_RETVAL_ERR						1
#define POST_RETVAL_COMPLETE				2

#define ONE_SEND_WSABUF_MAX					200

class MultiNetClient
{
public:
	MultiNetClient() = default;
	virtual ~MultiNetClient() = default;
	MultiNetClient(const MultiNetClient& other) = delete;
	MultiNetClient& operator=(const MultiNetClient& other) = delete;

public:
	bool SendPacket(NetBuffer& buffer);

	virtual void OnConnectionCompleted(MultiNetSessionId sessionId) = 0;
	virtual void OnReleased(MultiNetSessionId sessionId) = 0;

	virtual void OnRecv(MultiNetSessionId sessionId, NetBuffer& buffer) = 0;
	virtual void OnSend(MultiNetSessionId sessionId, int sendSize) = 0;

	virtual void OnWorkerThreadBegin() = 0;
	virtual void OnWorkerThreadEnd() = 0;
	virtual void OnError(st_Error& error) = 0;

protected:
	bool Start(const std::wstring& optionFile);
	void Stop();

	bool OptionParsing(const std::wstring& optionFile);

private:
	void WriteError(int windowError, int userError);

	bool ReleaseSession();
};