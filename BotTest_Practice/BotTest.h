#pragma once
#include "MultiNetClient.h"
#include "Bot.h"
#include <vector>

class BotTest : public MultiNetClient
{
private:
	BotTest() = default;
	~BotTest() = default;	
	BotTest(const BotTest& other) = delete; \
	BotTest& operator=(const BotTest& other) = delete; \

public:
	static BotTest& GetInstance();

public:
	bool Start(const std::wstring& optionFile);
	void Stop();

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
	void ProcessPacketHandle(const std::string& packetName, Bot& bot);
	void ProcessRecvPacketHandle(const std::string& packetName, Bot& bot, NetBuffer& packet);

private:
	std::vector<Bot::SPtr> botList;
};