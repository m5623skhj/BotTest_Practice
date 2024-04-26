#include "PreCompile.h"
#include "BotTest.h"

BotTest& BotTest::GetInstance()
{
	static BotTest instance;
	return instance;
}

void BotTest::OnConnected(MultiNetSessionId sessionId)
{

}

void BotTest::OnDisconnected(MultiNetSessionId sessionId)
{

}

void BotTest::OnReleased(MultiNetSessionId sessionId)
{

}

void BotTest::OnRecv(MultiNetSessionId sessionId, NetBuffer& buffer)
{

}

void BotTest::OnSend(MultiNetSessionId sessionId, int sendSize)
{

}

void BotTest::OnWorkerThreadBegin()
{

}

void BotTest::OnWorkerThreadEnd()
{

}

void BotTest::OnError(st_Error& error)
{

}