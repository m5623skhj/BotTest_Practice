#include "PreCompile.h"
#include "BotTest.h"
#include "PacketManager.h"
#include "Log.h"

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
	PacketId packetId;
	buffer >> packetId;

	auto packetName = PacketManager::GetInst().GetPacketName(packetId);
	if (packetName == std::nullopt)
	{
		std::cout << "Invalid packet id " << packetId << std::endl;
		return;
	}
	ProcessRecvPacketHandle(packetName.value(), *botList[sessionId], buffer);
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

void BotTest::ProcessPacketHandle(const std::string& packetName, Bot& bot)
{
	auto handler = PacketManager::GetInst().GetPacketHandler(packetName);
	if (handler != nullptr)
	{
		std::cout << "Invalid packet name " << packetName << std::endl;
		return;
	}

	handler(bot);
}

void BotTest::ProcessRecvPacketHandle(const std::string& packetName, Bot& bot, NetBuffer& packet)
{
	auto handler = PacketManager::GetInst().GetRecvPacketHandler(packetName);
	if (handler != nullptr)
	{
		std::cout << "Invalid packet " << packetName << std::endl;
		return;
	}

	handler(bot, packet);
}