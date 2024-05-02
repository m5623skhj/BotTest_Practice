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
	void AddScenarioIndex(ScenarioIndex addIndex);
	void SetScenarioIndex(ScenarioIndex setIndex);

	void OnScenarioCompleted();
	void OnTestCompleted();

public:
	void StopBot();
	bool IsStopCommandedBot();

private:
	MultiNetSessionId sessionId{};
	std::atomic_bool isConnected{};
	std::atomic_bool stopCommanded{};

private:
	ScenarioIndex scenarioIndex{};

#pragma region BotActionItems
public:
	void InitializeLoopCount();
	void AccumulateLoopCount();
	int GetLoopCount();

private:
	int loopCount = 0;

#pragma endregion BotActionItems
};