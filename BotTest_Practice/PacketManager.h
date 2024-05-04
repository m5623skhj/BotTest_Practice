#pragma once
#include "TestProtocol.h"
#include <unordered_map>
#include <functional>
#include "Bot.h"
#include <optional>

using PacketHandler = std::function<void(Bot&)>;
using RecvPacketHandler = std::function<void(Bot&, NetBuffer&)>;

class PacketManager
{
private:
	PacketManager();
	~PacketManager() = default;

	PacketManager(const PacketManager&) = delete;
	PacketManager& operator=(const PacketManager&) = delete;

public:
	static PacketManager& GetInst();

public:
	PacketHandler GetPacketHandler(PacketId packetId);
	RecvPacketHandler GetRecvPacketHandler(PacketId packetId);
	std::optional<std::string> GetPacketName(PacketId packetId);

private:
	std::unordered_map<PacketId, PacketHandler> packetHandlerMap;
	std::unordered_map<PacketId, RecvPacketHandler> recvPacketHandlerMap;
	std::unordered_map<PacketId, std::string> packetNameMap;

#pragma region PacketHandler
public:
	DECLARE_ALL_PACKET_HANDLER();
	DECLARE_ALL_RECV_PACKET_HANDLER();
#pragma endregion PacketHandler
};