#include "PreCompile.h"
#include "PacketManager.h"

PacketManager& PacketManager::GetInst()
{
	static PacketManager instance;
	return instance;
}

void PacketManager::Initialize()
{
	REGISTER_ALL_PACKET_HANDLER();
	REGISTER_ALL_RECV_PACKET_HANDLER();
	REGISTER_ALL_PACKET_NAME();
}

PacketHandler PacketManager::GetPacketHandler(PacketId packetId)
{
	auto iter = packetHandlerMap.find(packetId);
	if (iter == packetHandlerMap.end())
	{
		return nullptr;
	}

	return iter->second;
}

RecvPacketHandler PacketManager::GetRecvPacketHandler(PacketId packetId)
{
	auto iter = recvPacketHandlerMap.find(packetId);
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
