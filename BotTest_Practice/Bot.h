#pragma once
#include "MultiNetClientSession.h"
#include "Define.h"

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

public:
	ScenarioIndex GetScenarioIndex();
	void OnScenarioCompleted();
	void OnTestCompleted();

private:
	MultiNetSessionId sessionId{};
	std::atomic_bool isConnected{};

private:
	ScenarioIndex scenarioIndex{};
};