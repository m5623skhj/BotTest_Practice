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

	virtual bool InitAction(const nlohmann::json& json) { return true; }
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

#pragma region BotAction
struct BotAction_Ping : public IBotAction
{
	BOT_POST_ACTION DoAction(Bot& targetBot) override;
};

struct BotAction_TestStringPacket : public IBotAction
{
	BOT_POST_ACTION DoAction(Bot& targetBot) override;
};
#pragma endregion BotAction

#pragma region BotActionKeyword
struct BotActionKeyword_LoopStart : public IBotAction
{
	BOT_POST_ACTION DoAction(Bot& targetBot) override;
};

struct BotActionKeyword_LoopEnd : public IBotAction
{
	bool InitAction(const nlohmann::json& json);
	BOT_POST_ACTION DoAction(Bot& target) override;

private:
	int loopCount{};
	int jumpScenarioIndex{};
};
#pragma endregion BotActionKeyword

#define REGISTER_BOT_ACTION(ActionType, ActionString)\
	RegisterBotAction<ActionType>(#ActionString);

#define REGISTER_ALL_BOT_ACTION(){\
	REGISTER_BOT_ACTION(BotAction_Ping, Ping)\
	REGISTER_BOT_ACTION(BotActionKeyword_LoopStart, LoopStart)\
	REGISTER_BOT_ACTION(BotActionKeyword_LoopEnd, LoopEnd)\
	REGISTER_BOT_ACTION(BotAction_TestStringPacket, TestString)\
}

#pragma endregion BotAction