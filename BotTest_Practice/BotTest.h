#pragma once
#include "MultiNetClient.h"
#include "Bot.h"
#include <vector>
#include "TestProtocol.h"

class BotTest : public MultiNetClient
{
private:
	BotTest() = default;
	~BotTest() = default;	
	BotTest(const BotTest& other) = delete;
	BotTest& operator=(const BotTest& other) = delete;

public:
	static BotTest& GetInst();

public:
	bool Start(const std::wstring& optionFile);
	void Stop();

	bool SendPacket(NetBuffer& packet, MultiNetSessionId targetSessionId);

private:
	void OnConnected(MultiNetSessionId sessionId) override;
	void OnDisconnected(MultiNetSessionId sessionId) override;
	void OnReleased(MultiNetSessionId sessionId) override;
	
	void OnRecv(MultiNetSessionId sessionId, NetBuffer& buffer) override;
	void OnSend(MultiNetSessionId sessionId, int sendSize) override;

	void OnWorkerThreadBegin() override;
	void OnWorkerThreadEnd() override;
	void OnError(st_Error& error) override;

private:
	void ProcessPacketHandle(PacketId packetId, Bot& bot);
	void ProcessRecvPacketHandle(PacketId packetId, Bot& bot, NetBuffer& packet);

	void PrintInvalidPacketHandler(PacketId invalidPacketId);

private:
	std::vector<Bot::SPtr> botList;
};