#pragma once
#include "TestProtocol.h"
#include <unordered_map>
#include <functional>
#include "Bot.h"

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

private:
	std::unordered_map<std::string, PacketHandler> packetHandlerMap;

#pragma region PacketHandler
public:
	DECLARE_ALL_HANDLER();
#pragma endregion PacketHandler
};