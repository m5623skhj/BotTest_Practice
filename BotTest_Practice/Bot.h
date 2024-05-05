#pragma once
#include "MultiNetClientSession.h"
#include "Define.h"
#include <stack>

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

	MultiNetSessionId GetSessionId();

private:
	MultiNetSessionId sessionId{};
	std::atomic_bool isConnected{};
	std::atomic_bool stopCommanded{};

private:
	ScenarioIndex scenarioIndex{};

#pragma region BotActionItems
public:
	void SetTestString(const std::string& inString) { testString = inString; }
	std::string GetTestString() { return testString; }

private:
	std::string testString{};
#pragma endregion BotActionItems

#pragma region BotActionKeywordItems
public:
	void PushLoopCount();
	void PopLoopCount();
	void AccumulateLoopCount();
	int GetLoopCount();

private:
	std::stack<int> loopCountStack;

#pragma endregion BotActionKeywordItems
};