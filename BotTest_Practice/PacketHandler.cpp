#include "PreCompile.h"
#include "PacketManager.h"


bool PacketManager::HandlePacket(Bot& bot, TestStringPacket& packet)
{
	return true;
}

bool PacketManager::HandlePacket(Bot& bot, EchoStringPacket& packet)
{
	return true;
}

bool PacketManager::HandlePacket(Bot& bot, Ping& packet)
{
	return true;
}

bool PacketManager::HandlePacket(Bot& bot, Pong& packet)
{
	return true;
}