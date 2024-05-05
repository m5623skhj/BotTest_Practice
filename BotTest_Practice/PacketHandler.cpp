#include "PreCompile.h"
#include "PacketManager.h"
#include "BotTest.h"
#include "PacketHandlerUtil.h"

bool PacketManager::HandleTestStringPacket(Bot& bot)
{
	std::string randomString = PacketHandlerUtil::MakeRandomString(20);

	auto& packet = *NetBuffer::Alloc();
	auto packetId = PACKET_ID::TEST_STRING_PACKET;
	bot.SetTestString(randomString);
	packet << packetId << randomString;

	return BotTest::GetInst().SendPacket(packet, bot.GetSessionId());
}

bool PacketManager::HandleEchoStringPacket(Bot& bot, NetBuffer& packet)
{
	std::string recvString{};

	packet >> recvString;

	return recvString == bot.GetTestString();
}

bool PacketManager::HandlePing(Bot& bot)
{
	auto& packet = *NetBuffer::Alloc();
	auto packetId = PACKET_ID::PING;
	packet << packetId;

	return BotTest::GetInst().SendPacket(packet, bot.GetSessionId());
}

bool PacketManager::HandlePong(Bot& bot, NetBuffer& packet)
{
	return true;
}