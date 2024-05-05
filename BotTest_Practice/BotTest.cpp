#include "PreCompile.h"
#include "BotTest.h"
#include "PacketManager.h"
#include "BotActionManager.h"
#include "PacketManager.h"

BotTest& BotTest::GetInst()
{
	static BotTest instance;
	return instance;
}

bool BotTest::Start(const std::wstring& optionFile)
{
	PacketManager::GetInst().Initialize();

	if (BotActionManager::GetInst().Initialize() == false)
	{
		return false;
	}

	if (MultiNetClient::Start(optionFile) == false)
	{
		return false;
	}

	return true;
}

void BotTest::Stop()
{
	MultiNetClient::Stop();
}

bool BotTest::SendPacket(NetBuffer& packet, MultiNetSessionId targetSessionId)
{
	return MultiNetClient::SendPacket(packet, targetSessionId);
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

	if (botList[sessionId]->IsStopCommandedBot())
	{
		return;
	}

	ProcessRecvPacketHandle(packetId, *botList[sessionId], buffer);
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

void BotTest::ProcessPacketHandle(PacketId packetId, Bot& bot)
{
	auto handler = PacketManager::GetInst().GetPacketHandler(packetId);
	if (handler == nullptr)
	{
		PrintInvalidPacketHandler(packetId);
		return;
	}

	handler(bot);
}

void BotTest::ProcessRecvPacketHandle(PacketId packetId, Bot& bot, NetBuffer& packet)
{
	auto handler = PacketManager::GetInst().GetRecvPacketHandler(packetId);
	if (handler == nullptr)
	{
		PrintInvalidPacketHandler(packetId);
		return;
	}

	handler(bot, packet);
}

void BotTest::PrintInvalidPacketHandler(PacketId invalidPacketId)
{
	auto packetName = PacketManager::GetInst().GetPacketName(invalidPacketId);
	if (packetName == std::nullopt)
	{
		std::cout << "Invalid packet name " << packetName.value() << std::endl;
	}
	else
	{
		std::cout << "Invalid packet id " << invalidPacketId << std::endl;
	}
}