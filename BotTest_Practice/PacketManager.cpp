#include "PreCompile.h"
#include "PacketManager.h"

PacketManager& PacketManager::GetInst()
{
	static PacketManager instance;
	return instance;
}

PacketHandler PacketManager::GetPacketHandler(std::string packetName)
{
	auto iter = packetHandlerMap.find(packetName);
	if (iter == packetHandlerMap.end())
	{
		return nullptr;
	}

	return iter->second;
}