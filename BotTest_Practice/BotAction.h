#pragma once
#include "nlohmann/json.hpp"
#include "Define.h"

class Bot;
class BotActionManager;

enum BOT_POST_ACTION
{
	NORMAL,
	DO_NEXT_IMMEDIATLY,
	STOP
};

#pragma region BotAction
struct IBotAction
{
	friend BotActionManager;

	virtual void InitAction(const nlohmann::json& json) {}
	virtual BOT_POST_ACTION DoAction(Bot& targetBot) = 0;

public:
	ScenarioIndex GetScenarioIndex()
	{
		return scenarioIndex;
	}

private:
	void SetScenarioIndex(ScenarioIndex inScenarioIndex)
	{
		scenarioIndex = inScenarioIndex;
	}

protected:
	ScenarioIndex scenarioIndex{};
};

struct BotAction_Ping : public IBotAction
{
	BOT_POST_ACTION DoAction(Bot& targetBot) override;
};

struct BotActionKeyword_LoopStart : public IBotAction
{
	void InitAction(const nlohmann::json& json);
	BOT_POST_ACTION DoAction(Bot& targetBot) override;
};

struct BotActionKeyword_LoopEnd : public IBotAction
{
	void InitAction(const nlohmann::json& json);
	BOT_POST_ACTION DoAction(Bot& target) override;

private:
	int loopCount{};
	int jumpScenarioIndex{};
};

#define REGISTER_BOT_ACTION(ActionType, ActionString)\
	RegisterBotAction<ActionType>(#ActionString);

#define REGISTER_ALL_BOT_ACTION(){\
	REGISTER_BOT_ACTION(BotAction_Ping, Ping)\
	REGISTER_BOT_ACTION(BotActionKeyword_LoopStart, LoopStart)\
	REGISTER_BOT_ACTION(BotActionKeyword_LoopEnd, LoopEnd)\
}

#pragma endregion BotAction