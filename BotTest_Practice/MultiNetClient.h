#pragma once
#include "Ringbuffer.h"
#include "LockFreeStack.h"
#include "LockFreeQueue.h"
#include "ServerCommon.h"
#include "NetServerSerializeBuffer.h"
#include "MultiNetClientSession.h"
#include <vector>
#include <shared_mutex>

#define POST_RETVAL_ERR_SESSION_DELETED		0
#define POST_RETVAL_ERR						1
#define POST_RETVAL_COMPLETE				2

#define ONE_SEND_WSABUF_MAX					200

class MultiNetClientSession;

class MultiNetClient
{
public:
	MultiNetClient() = default;
	virtual ~MultiNetClient() = default;
	MultiNetClient(const MultiNetClient& other) = delete;
	MultiNetClient& operator=(const MultiNetClient& other) = delete;

public:
	bool SendPacket(NetBuffer& buffer);

	virtual void OnConnected(MultiNetSessionId sessionId) = 0;
	virtual void OnDisconnected(MultiNetSessionId sessionId) = 0;
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
	bool ReleaseSession(MultiNetClientSession& session);
	bool MakeThreads();
	bool MakeSessionList();

private:
	void WriteError(int windowError, int userError);

private:
	static UINT __stdcall WorkerThread(LPVOID pLanClient);
	UINT Worker();
	bool WorkerImpl();

	static UINT __stdcall ReconnecterThread(LPVOID pLanClient);
	UINT Reconnecter();
	bool ReconnecterImpl(const SOCKADDR_IN& serverAddr);
	bool ReconnecterImplConnectLoop(MultiNetSessionId releasedSessionId, const SOCKADDR_IN& serverAddr);

	char IOCPRecvCompleted(MultiNetClientSession& session, DWORD transferred);
	char IOCPSendCompleted(MultiNetClientSession& session);

	bool GenerateRecvCompletedPacket(MultiNetClientSession& session, OUT int& ringBufferRestSize, OUT NetBuffer& recvPacket);

private:
	char RecvPost(MultiNetClientSession& session);
	char SendPost(MultiNetClientSession& session);

	void MakeWSARecvBuffer(MultiNetClientSession& session, OUT int& bufferCount, OUT WSABUF (&wsaBuffer)[2]);
	void MakeWSASendBuffer(MultiNetClientSession& session, OUT int& useSize, OUT WSABUF (&wsaBuffer)[ONE_SEND_WSABUF_MAX]);

private:
	std::shared_mutex sessionListLock;
	std::vector<std::shared_ptr<MultiNetClientSession>> sessionList;

	std::mutex sessionReconnectIdListLock;
	std::list<MultiNetSessionId> sessionReconnectIdList;

private:
	WCHAR ip[16];
	WORD port;
	BYTE nagleOn;
	BYTE reconnectDiconnectedSession;
	std::atomic_uint reconnectCount{};

	BYTE numOfWorkerThreads;
	BYTE numOfUsingWorkerThreads;

	WORD numOfSession;
	std::atomic<UINT64> m_sessionIdGenerator{};

	bool stopClient{};

private:
	HANDLE *workerThreadsHandle;
	HANDLE workerIOCP;
	HANDLE reconnecterHandle;

private:
	UINT numOfReconnect{};
};