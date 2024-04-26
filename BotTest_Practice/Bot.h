#pragma once
#include "MultiNetClientSession.h"

class Bot
{
public:
	using SPtr = std::shared_ptr<Bot>;

public:
	Bot() = delete;
	Bot(MultiNetSessionId inSessionId);
	~Bot() = default;

public:
	void OnConnected();
	void OnDisconnected();

private:
	MultiNetSessionId sessionId{};
	std::atomic_bool isConnected{};
};