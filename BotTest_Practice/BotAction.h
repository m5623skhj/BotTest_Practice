#pragma once
#include "nlohmann/json.hpp"

class Bot;

#pragma region BotAction
struct IBotAction
{
	virtual void InitAction(const nlohmann::json& json) {}
	virtual void DoAction(Bot& targetBot) = 0;
};

struct BotAction_Ping : public IBotAction
{
	void DoAction(Bot& targetBot) override;
};

struct BotActionKeyword_LoopStart : public IBotAction
{
	void InitAction(const nlohmann::json& json);
	void DoAction(Bot& targetBot) override;

private:
	int loopCount = 0;
};

struct BotActionKeyword_LoopEnd : public IBotAction
{
	void InitAction(const nlohmann::json& json);
	void DoAction(Bot& target) override;
};

#define REGISTER_BOT_ACTION(ActionType, ActionString)\
	RegisterBotAction<ActionType>(#ActionString);

#define REGISTER_ALL_BOT_ACTION(){\
	REGISTER_BOT_ACTION(BotAction_Ping, Ping)\
	REGISTER_BOT_ACTION(BotActionKeyword_LoopStart, LoopStart)\
	REGISTER_BOT_ACTION(BotActionKeyword_LoopEnd, LoopEnd)\
}

#pragma endregion BotAction