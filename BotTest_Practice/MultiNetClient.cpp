#include "PreCompile.h"
#include "MultiNetClient.h"
#include "Log.h"
#include "MultiNetClientSession.h"

#define CHECK_ERROR(Func, FailedResult, FailedServerErrorType) \
if (Func == FailedResult) \
{ \
	WriteError(WSAGetLastError(), FailedServerErrorType); \
	return false; \
}

bool MultiNetClient::SendPacket(NetBuffer& buffer)
{
	return true;
}

bool MultiNetClient::Start(const std::wstring& optionFile)
{
	CHECK_ERROR(OptionParsing(optionFile), false, SERVER_ERR::PARSING_ERR);
	for (WORD i = 0; i < numOfSession; ++i)
	{
		sessionList.emplace_back(std::make_shared<MultiNetClientSession>());
	}

	WSADATA wsaData;
	auto version = MAKEWORD(2, 2);
	CHECK_ERROR(WSAStartup(version, &wsaData), 0, SERVER_ERR::WSASTARTUP_ERR);

	CHECK_ERROR(MakeThreads(), false, SERVER_ERR::MAKE_THREADS_ERROR);
	CHECK_ERROR(MakeSessionList(), false, SERVER_ERR::MAKE_SESSION_LIST_ERROR);

	return true;
}

void MultiNetClient::Stop()
{
	stopClient = true;

	for (auto& session : sessionList)
	{
		shutdown(session->socket, SD_BOTH);
	}

	PostQueuedCompletionStatus(workerIOCP, 0, 0, (LPOVERLAPPED)1);
	WaitForMultipleObjects(numOfWorkerThreads, workerThreadsHandle, TRUE, INFINITE);

	for (BYTE i = 0; i < numOfWorkerThreads; ++i)
	{
		CloseHandle(workerThreadsHandle[i]);
	}
	CloseHandle(workerIOCP);

	WSACleanup();
}

bool MultiNetClient::OptionParsing(const std::wstring& optionFile)
{
	_wsetlocale(LC_ALL, L"Korean");

	CParser parser;
	WCHAR cBuffer[BUFFER_MAX];

	FILE* fp;
	_wfopen_s(&fp, optionFile.c_str(), L"rt, ccs=UNICODE");
	if (fp == NULL)
		return false;

	int iJumpBOM = ftell(fp);
	fseek(fp, 0, SEEK_END);
	int iFileSize = ftell(fp);
	fseek(fp, iJumpBOM, SEEK_SET);
	int FileSize = (int)fread_s(cBuffer, BUFFER_MAX, sizeof(WCHAR), BUFFER_MAX / 2, fp);
	int iAmend = iFileSize - FileSize; // 개행 문자와 파일 사이즈에 대한 보정값
	fclose(fp);

	cBuffer[iFileSize - iAmend] = '\0';
	WCHAR* pBuff = cBuffer;

	BYTE HeaderCode, XORCode, DebugLevel;

	if (!parser.GetValue_String(pBuff, L"NETCLIENT", L"SERVER_IP", ip))
		return false;
	if (!parser.GetValue_Short(pBuff, L"NETCLIENT", L"SERVER_PORT", (short*)&port))
		return false;
	if (!parser.GetValue_Byte(pBuff, L"NETCLIENT", L"WORKER_THREAD", &numOfWorkerThreads))
		return false;
	if (!parser.GetValue_Byte(pBuff, L"NETCLIENT", L"USE_IOCPWORKER", &numOfUsingWorkerThreads))
		return false;
	if (!parser.GetValue_Byte(pBuff, L"NETCLIENT", L"NAGLE_ON", (BYTE*)&nagleOn))
		return false;
	if (!parser.GetValue_Byte(pBuff, L"NETCLIENT", L"RECONNECT", (BYTE*)&reconnectDiconnectedSession))
		return false;
	if (!parser.GetValue_Byte(pBuff, L"SERIALIZEBUF", L"PACKET_CODE", &HeaderCode))
		return false;
	if (!parser.GetValue_Byte(pBuff, L"SERIALIZEBUF", L"PACKET_KEY", &XORCode))
		return false;
	if (!parser.GetValue_Byte(pBuff, L"OPTION", L"LOG_LEVEL", &DebugLevel))
		return false;
	if (!parser.GetValue_Short(pBuff, L"LANCLIENT", L"SESSION_COUNT", (short*)&numOfSession))
		return false;

	NetBuffer::m_byHeaderCode = HeaderCode;
	NetBuffer::m_byXORCode = XORCode;
	SetLogLevel(DebugLevel);

	return true;
}

bool MultiNetClient::ReleaseSession(MultiNetClientSession& session)
{
	int sendBufferRestSize = session.sendIOItem.bufferCount;
	int rest = session.sendIOItem.sendQ.GetRestSize();

	for (int i = 0; i < session.sendIOItem.bufferCount; ++i)
	{
		NetBuffer::Free(session.sendBufferStore[i]);
	}

	if (rest > 0)
	{
		NetBuffer* buffer;
		for (int i = 0; i < rest; ++i)
		{
			session.sendIOItem.sendQ.Dequeue(&buffer);
			NetBuffer::Free(buffer);
		}
	}

	closesocket(session.socket);
	
	session.isConnected = false;
	OnDisconnected(session.GetSessionId());

	if (reconnectDiconnectedSession == false)
	{
		return true;
	}

	{
		std::scoped_lock<std::shared_mutex> lock(sessionReconnectListLock);
		sessionReconnectList.emplace_back(session.sessionId);
	}

	return true;
}

bool MultiNetClient::MakeThreads()
{
	workerThreadsHandle = new HANDLE[numOfWorkerThreads];

	workerIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numOfWorkerThreads);
	if (workerIOCP == NULL)
	{
		return false;
	}

	for (int i = 0; i < numOfWorkerThreads; i++)
	{
		workerThreadsHandle[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, NULL);
	}

	if (workerThreadsHandle == 0)
	{
		return false;
	}
	reconnecterHandle = (HANDLE)_beginthreadex(NULL, 0, ReconnecterThread, this, 0, NULL);

	return true;
}

bool MultiNetClient::MakeSessionList()
{
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	InetPton(AF_INET, ip, &serverAddr.sin_addr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	int returnValue;

	for (auto& session : sessionList)
	{
		session->socket = socket(AF_INET, SOCK_STREAM, 0);
		if (session->socket == INVALID_SOCKET)
		{
			return false;
		}

		while (1)
		{
			if (connect(session->socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) != SOCKET_ERROR)
				break;
			if (GetLastError() == WSAEISCONN)
			{
				closesocket(session->socket);

				session->socket = socket(AF_INET, SOCK_STREAM, 0);
				if (session->socket == INVALID_SOCKET)
				{
					return false;
				}
			}

			Sleep(1000);
		}
		session->isConnected = true;

		returnValue = setsockopt(session->socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&nagleOn, sizeof(int));
		if (returnValue == SOCKET_ERROR)
		{
			WriteError(WSAGetLastError(), SERVER_ERR::SETSOCKOPT_ERR);
			return false;
		}

		ZeroMemory(&session->sendIOItem.overlapped, sizeof(OVERLAPPED));
		ZeroMemory(&session->recvIOItem.overlapped, sizeof(OVERLAPPED));

		CreateIoCompletionPort((HANDLE)session->socket, workerIOCP, (ULONG_PTR)&session, 0);

		session->sendIOItem.bufferCount = 0;
		session->sendIOItem.ioMode = false;

		session->sessionId = m_sessionIdGenerator;
		++m_sessionIdGenerator;

		RecvPost(*session);
		OnConnected(session->GetSessionId());
	}

	return true;
}

void MultiNetClient::WriteError(int windowError, int userError)
{
	st_Error Error;
	Error.GetLastErr = windowError;
	Error.ServerErr = userError;
	OnError(Error);
}

UINT __stdcall MultiNetClient::WorkerThread(LPVOID netClient)
{
	return ((MultiNetClient*)netClient)->Worker();
}

UINT MultiNetClient::Worker()
{
	srand((ULONGLONG)this);
	while (true)
	{
		if (WorkerImpl() == false)
		{
			break;
		}
	}

	NetBuffer::ChunkFreeForcibly();
	return 0;
}

bool MultiNetClient::WorkerImpl()
{
	char postReturnValue;
	DWORD transferred;
	MultiNetClientSession** pSession;
	LPOVERLAPPED overlapped;

	if (GetQueuedCompletionStatus(workerIOCP, &transferred, (PULONG_PTR)&pSession, &overlapped, INFINITE) == FALSE)
	{
		WriteError(WSAGetLastError(), SERVER_ERR::GQCS_FAILED);
		g_Dump.Crash();
	}

	OnWorkerThreadBegin();
	if (overlapped == NULL)
	{
		WriteError(WSAGetLastError(), SERVER_ERR::OVERLAPPED_NULL_ERR);
		g_Dump.Crash();
	}
	if (pSession == NULL)
	{
		PostQueuedCompletionStatus(workerIOCP, 0, 0, NULL);
		return false;
	}
	if (*pSession == nullptr)
	{
		g_Dump.Crash();
	}

	auto& session = **pSession;
	if (overlapped == &session.recvIOItem.overlapped)
	{
		if (transferred == 0)
		{
			if (--session.ioCount == 0)
			{
				ReleaseSession(session);
			}
			return true;
		}
		postReturnValue = IOCPRecvCompleted(session, transferred);
	}
	else if (overlapped == &session.sendIOItem.overlapped)
	{
		postReturnValue = IOCPSendCompleted(session);
	}

	OnWorkerThreadEnd();
	if (postReturnValue == POST_RETVAL_ERR_SESSION_DELETED)
	{
		return true;
	}
	if (--session.ioCount == 0)
	{
		ReleaseSession(session);
	}

	return true;
}

UINT __stdcall MultiNetClient::ReconnecterThread(LPVOID netClient)
{
	return ((MultiNetClient*)netClient)->Reconnecter();
}

UINT MultiNetClient::Reconnecter()
{
	return 0;
}

char MultiNetClient::IOCPRecvCompleted(MultiNetClientSession& session, DWORD transferred)
{
	session.recvIOItem.ringBuffer.MoveWritePos(transferred);
	int ringBufferRestSize = session.recvIOItem.ringBuffer.GetUseSize();
	bool isOccurredPacketError = false;
	int bufferDequeueSize = 0;

	while (ringBufferRestSize > df_HEADER_SIZE)
	{
		NetBuffer& recvSerializeBuf = *NetBuffer::Alloc();
		session.recvIOItem.ringBuffer.Peek((char*)recvSerializeBuf.m_pSerializeBuffer, df_HEADER_SIZE);
		recvSerializeBuf.m_iRead = 0;

		BYTE code;
		WORD payloadLength;
		recvSerializeBuf >> code >> payloadLength;
		if (code != NetBuffer::m_byHeaderCode)
		{
			isOccurredPacketError = true;
			WriteError(0, SERVER_ERR::HEADER_CODE_ERR);
			NetBuffer::Free(&recvSerializeBuf);
			break;
		}
		if (ringBufferRestSize < payloadLength + df_HEADER_SIZE)
		{
			if (payloadLength > dfDEFAULTSIZE)
			{
				isOccurredPacketError = true;
				WriteError(0, SERVER_ERR::PAYLOAD_SIZE_OVER_ERR);
			}
			NetBuffer::Free(&recvSerializeBuf);
			break;
		}
		session.recvIOItem.ringBuffer.RemoveData(df_HEADER_SIZE);

		bufferDequeueSize = session.recvIOItem.ringBuffer.Dequeue(&recvSerializeBuf.m_pSerializeBuffer[recvSerializeBuf.m_iWrite], payloadLength);
		recvSerializeBuf.m_iWrite += bufferDequeueSize;
		if (!recvSerializeBuf.Decode())
		{
			isOccurredPacketError = true;
			WriteError(0, SERVER_ERR::CHECKSUM_ERR);
			NetBuffer::Free(&recvSerializeBuf);
			break;
		}

		ringBufferRestSize -= (bufferDequeueSize + df_HEADER_SIZE);
		OnRecv(session.GetSessionId(), recvSerializeBuf);
		NetBuffer::Free(&recvSerializeBuf);
	}

	return RecvPost(session);
}

char MultiNetClient::IOCPSendCompleted(MultiNetClientSession& session)
{
	int bufferCount = session.sendIOItem.bufferCount;
	for (int i = 0; i < bufferCount; ++i)
	{
		NetBuffer::Free(session.sendBufferStore[i]);
	}

	session.sendIOItem.bufferCount -= bufferCount;

	OnSend(session.GetSessionId(), bufferCount);
	session.sendIOItem.ioMode = false;
	return SendPost(session);
}

char MultiNetClient::RecvPost(MultiNetClientSession& session)
{
	if (session.recvIOItem.ringBuffer.IsFull() == true)
	{
		if (--session.ioCount == 0)
		{
			ReleaseSession(session);
			return POST_RETVAL_ERR_SESSION_DELETED;
		}
		return POST_RETVAL_ERR;
	}

	int bufferCount = 1;
	WSABUF wsaBuffer[2];
	DWORD flag = 0;
	MakeWSARecvBuffer(session, bufferCount, wsaBuffer);

	++session.ioCount;
	if (WSARecv(session.socket, wsaBuffer, bufferCount, NULL, &flag, &session.recvIOItem.overlapped, 0) == SOCKET_ERROR)
	{
		int wsaError = WSAGetLastError();
		if (wsaError != ERROR_IO_PENDING)
		{
			if (--session.ioCount == 0)
			{
				ReleaseSession(session);
				return POST_RETVAL_ERR_SESSION_DELETED;
			}

			WriteError(wsaError, SERVER_ERR::WSARECV_ERR);
			return POST_RETVAL_ERR;
		}
	}

	return POST_RETVAL_COMPLETE;
}

char MultiNetClient::SendPost(MultiNetClientSession& session)
{
	while (1)
	{
		bool expected = false;
		if (session.sendIOItem.ioMode.compare_exchange_strong(expected, true) == false)
		{
			return POST_RETVAL_COMPLETE;
		}

		int useSize = session.sendIOItem.sendQ.GetRestSize();
		if (useSize == 0)
		{
			session.sendIOItem.ioMode = false;
			if (session.sendIOItem.sendQ.GetRestSize() > 0)
			{
				continue;
			}
			return POST_RETVAL_COMPLETE;
		}
		else if (useSize < 0)
		{
			if (--session.ioCount == 0)
			{
				ReleaseSession(session);
				return POST_RETVAL_ERR_SESSION_DELETED;
			}
			session.sendIOItem.ioMode = false;
			return POST_RETVAL_ERR;
		}

		WSABUF wsaBuffer[ONE_SEND_WSABUF_MAX];
		MakeWSASendBuffer(session, useSize, wsaBuffer);

		++session.ioCount;
		if (WSASend(session.socket, wsaBuffer, useSize, NULL, 0, &session.sendIOItem.overlapped, 0) == SOCKET_ERROR)
		{
			int wsaError = WSAGetLastError();
			if (wsaError == ERROR_IO_PENDING)
			{
				continue;
			}

			if (wsaError == WSAENOBUFS)
			{
				g_Dump.Crash();
			}

			WriteError(wsaError, SERVER_ERR::WSASEND_ERR);
			if (--session.ioCount == 0)
			{
				ReleaseSession(session);
				return POST_RETVAL_ERR_SESSION_DELETED;
			}
			return POST_RETVAL_ERR;
		}
	}
}

void MultiNetClient::MakeWSARecvBuffer(MultiNetClientSession& session, OUT int& bufferCount, OUT WSABUF(&wsaBuffer)[2])
{
	int brokenSize = session.recvIOItem.ringBuffer.GetNotBrokenPutSize();
	int restSize = session.recvIOItem.ringBuffer.GetFreeSize() - brokenSize;

	wsaBuffer[0].buf = session.recvIOItem.ringBuffer.GetWriteBufferPtr();
	wsaBuffer[0].len = brokenSize;
	if (restSize > 0)
	{
		wsaBuffer[1].buf = session.recvIOItem.ringBuffer.GetBufferPtr();
		wsaBuffer[1].len = restSize;
		++bufferCount;
	}
}

void MultiNetClient::MakeWSASendBuffer(MultiNetClientSession& session, OUT int& useSize, OUT WSABUF(&wsaBuffer)[ONE_SEND_WSABUF_MAX])
{
	if (useSize < ONE_SEND_WSABUF_MAX)
	{
		useSize = ONE_SEND_WSABUF_MAX;
	}

	for (int i = 0; i < useSize; ++i)
	{
		session.sendIOItem.sendQ.Dequeue(&session.sendBufferStore[i]);
		wsaBuffer[i].buf = session.sendBufferStore[i]->GetBufferPtr();
		wsaBuffer[i].len = session.sendBufferStore[i]->GetAllUseSize();
	}
	session.sendIOItem.bufferCount = useSize;

}
