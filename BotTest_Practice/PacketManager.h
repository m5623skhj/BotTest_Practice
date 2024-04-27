#pragma once
#include "TestProtocol.h"
#include <unordered_map>
#include <functional>
#include "Bot.h"
#include <optional>

using PacketHandler = std::function<void()>;

class PacketManager
{
private:
	PacketManager() = default;
	~PacketManager() = default;

	PacketManager(const PacketManager&) = delete;
	PacketManager& operator=(const PacketManager&) = delete;

public:
	static PacketManager& GetInst();

public:
	PacketHandler GetPacketHandler(std::string packetName);
	std::optional<std::string> GetPacketName(PacketId packetId);

private:
	std::unordered_map<std::string, PacketHandler> packetHandlerMap;
	std::unordered_map<PacketId, std::string> packetNameMap;

#pragma region PacketHandler
public:
	DECLARE_ALL_HANDLER();
#pragma endregion PacketHandler
};