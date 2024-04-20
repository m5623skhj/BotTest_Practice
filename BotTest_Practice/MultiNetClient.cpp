#include "PreCompile.h"
#include "MultiNetClient.h"
#include "Log.h"

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

void MultiNetClient::OnConnectionCompleted(MultiNetSessionId sessionId)
{

}

void MultiNetClient::OnReleased(MultiNetSessionId sessionId)
{

}

void MultiNetClient::OnRecv(MultiNetSessionId sessionId, NetBuffer& buffer)
{

}

void MultiNetClient::OnSend(MultiNetSessionId sessionId, int sendSize)
{

}

void MultiNetClient::OnWorkerThreadBegin()
{

}

void MultiNetClient::OnWorkerThreadEnd()
{

}

void MultiNetClient::OnError(st_Error& error)
{

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

	CNetServerSerializationBuf::m_byHeaderCode = HeaderCode;
	CNetServerSerializationBuf::m_byXORCode = XORCode;
	SetLogLevel(DebugLevel);

	return true;
}

bool MultiNetClient::ReleaseSession()
{
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
		session->sendIOItem.ioMode = NONSENDING;

		session->sessionId = m_sessionIdGenerator;
		++m_sessionIdGenerator;

		RecvPost(*session);
		OnConnectionCompleted(session->GetSessionId());
	}

	return true;
}

void MultiNetClient::WriteError(int windowError, int userError)
{

}

UINT __stdcall MultiNetClient::WorkerThread(LPVOID netClient)
{
	return ((MultiNetClient*)netClient)->Worker();
}

UINT MultiNetClient::Worker()
{
	return 0;
}

UINT __stdcall MultiNetClient::ReconnecterThread(LPVOID netClient)
{
	return ((MultiNetClient*)netClient)->Reconnecter();
}

UINT MultiNetClient::Reconnecter()
{
	return 0;
}

char MultiNetClient::RecvPost(MultiNetClientSession& session)
{
	return 0;
}

char MultiNetClient::SendPost(MultiNetClientSession& session)
{
	return 0;
}