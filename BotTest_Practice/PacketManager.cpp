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

RecvPacketHandler PacketManager::GetRecvPacketHandler(std::string packetName)
{
	auto iter = recvPacketHandlerMap.find(packetName);
	if (iter == recvPacketHandlerMap.end())
	{
		return nullptr;
	}

	return iter->second;
}

std::optional<std::string> PacketManager::GetPacketName(PacketId packetId)
{
	auto iter = packetNameMap.find(packetId);
	if (iter == packetNameMap.end())
	{
		return std::nullopt;
	}

	return iter->second;
}
